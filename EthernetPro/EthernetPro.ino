#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xCB, 0x51 };
IPAddress ip(10, 0, 1, 25);
char server[] = "jimmybyrum.com";

boolean hasEthernet = false;
int ethernetFailures = 0;

EthernetClient client;

unsigned long lastConnectionTime = 0;
boolean lastConnected = false;
const unsigned long postingInterval = 5 * 1000;

void setup() {
  Serial.begin(9600);
  Serial.println("Serial begin");
  delay(1000);
}

void loop() {
  long timePassed = millis() - lastConnectionTime;
  if (!client.connected() && timePassed > postingInterval) {
    Serial.println("--------------");
    httpRequest();
  }
  lastConnected = client.connected();
  
  delay(50);
}


void resetArduino() {
  Serial.println("RESET");
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
      if (ethernetFailures >= 1) {
        resetArduino();
      }
      Serial.println("[Ethernet failed] " + String(ethernetFailures));
    }
  }
}

void httpRequest() {
  startEthernet();
  if (hasEthernet && client.connect(server, 80)) {
    String s = "GET /340HarvardSt/?v=0.1";
    s += " HTTP/1.1";
    Serial.println(s);
    client.println(s);
    client.println("Host: jimmybyrum.com");
    client.println("User-Agent: arduino-pro");
    client.println("Connection: close");
    client.println();
  } else {
    hasEthernet = false;
    Serial.println("FAILED");
  }
  client.stop();
  Serial.println("--------------");
  lastConnectionTime = millis();
}


