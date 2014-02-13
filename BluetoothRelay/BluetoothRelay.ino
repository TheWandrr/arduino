#include <SPI.h>
#include <Ethernet.h>
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xCB, 0x53 };
//IPAddress ip(10, 0, 1, 25);
boolean hasEthernet = false;
int ethernetFailures = 0;
EthernetClient client;
char server[] = "tbea.local";
//char server[] = "jimmybyrum.com";
unsigned long lastConnectionTime = 0;
boolean lastConnected = false;
const unsigned long postingInterval = 5 * 1000;

#include <SoftwareSerial.h>
int bluetoothTx = 0;
int bluetoothRx = 1;
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

int statusLed = 5;

void setup() {
  pinMode(statusLed, OUTPUT);

  pinMode(bluetoothRx, INPUT);
  pinMode(bluetoothTx, OUTPUT);

  Serial.begin(9600);
  //while( ! Serial) ;

  delay(1000);

  bluetooth.begin(115200);
  bluetooth.print("$$$");
  delay(100);
  bluetooth.println("+INQ=1");
}

void loop() {
  if (bluetooth.available()) {
    char inBt = bluetooth.read();
    process(inBt);
  }
  if (Serial.available()) {
    char inChar = Serial.read();
    process(inChar);
  }
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  } else if ( ! client.connected()) {
    Serial.println();
    client.stop();
  }
}

void ledAlert(int blinks, int delay_length) {
  for (int i = 0; i < blinks; i++) {
    digitalWrite(statusLed, HIGH);
    delay(delay_length);
    digitalWrite(statusLed, LOW);
    delay(delay_length);
  }
}

void startEthernet() {
  if ( ! hasEthernet) {
    if (Ethernet.begin(mac) > 0) {
      hasEthernet = true;
      ethernetFailures = 0;
      Serial.println("[Ethernet using DHCP]");
    } else {
      hasEthernet = false;
      ethernetFailures++;
      Serial.println("[Ethernet failed] " + String(ethernetFailures));
    }
  }
}

void httpRequest(String command) {
  Serial.println("--------------");
  startEthernet();
  if (hasEthernet && client.connect(server, 80)) {
    Serial.println(server);
    String s = "GET /arduino/servo/" + command + "/";
    s += " HTTP/1.1";
    Serial.println(s);
    client.println(s);
    client.println("Host: " + String(server));
    client.println("User-Agent: arduino-pro");
    client.println("Connection: close");
    client.println();
    ledAlert(6, 100);
  } else {
    hasEthernet = false;
    Serial.println("FAILED");
    ledAlert(6, 500);
    client.stop();
  }
  Serial.println("--------------");
  lastConnectionTime = millis();
  delay(500);
}

void process(char inChar) {
  switch (inChar) {
    case '1':
      digitalWrite(13, HIGH);
      httpRequest("open");
      break;
    case '0':
    case '2':
      digitalWrite(13, LOW);
      httpRequest("close");
      break;
  }
}

