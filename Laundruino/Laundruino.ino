#include <SPI.h>
#include <Ethernet.h>
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xCB, 0x53 };
//IPAddress ip(10, 0, 1, 25);
boolean hasEthernet = false;
int ethernetFailures = 0;
EthernetClient client;
char server[] = "jimmybyrum.com";

unsigned long lastConnectionTime = 0;
boolean lastConnected = false;
const unsigned long postingInterval = 5*1000;



#include <Wire.h>
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
int Addr = 105; // I2C address of gyro
int x, y, z;

const int numGyroReadings = 50;
float gyroReadings[numGyroReadings];
int gyroIndex = 0;
int valX = 0;
int valY = 0;
int valZ = 0;
int gyroTotal = 0;
int gyroAverage = 0;

int resetPin = 12;
int statusLed = 13;


void setup() {
  digitalWrite(resetPin, HIGH);
  delay(200);

  Wire.begin();

  pinMode(resetPin, OUTPUT);
  pinMode(statusLed, OUTPUT);

  Serial.begin(9600);
  //  while (!Serial) ;
  Serial.println("Starting");

  writeI2C(CTRL_REG1, 0x1F);    // Turn on all axes, disable power down
  writeI2C(CTRL_REG3, 0x08);    // Enable control ready signal
  writeI2C(CTRL_REG4, 0x80);    // Set scale (500 deg/sec)
  delay(100);                   // Wait to synchronize

  for (int i = 0; i < numGyroReadings; i++) {
    gyroReadings[i] = 0;
  }

  Serial.println("Ready");
}

void loop() {
  String s = "";

  getGyroValues();
  // In following Dividing by 114 reduces noise
  valX = abs(x);
  valY = abs(y);
  valZ = abs(z);
  int gyroVals = valX + valY + valZ;

  gyroTotal = gyroTotal - gyroReadings[gyroIndex];
  gyroReadings[gyroIndex] = gyroVals;
  gyroTotal = gyroTotal + gyroReadings[gyroIndex];
  gyroIndex = gyroIndex + 1;
  if (gyroIndex >= numGyroReadings) {
    gyroIndex = 0;
  }
  gyroAverage = abs(gyroTotal / numGyroReadings);

  s += "X:" + String(valX);
  s += "\tY:" + String(valY);
  s += "\tZ:" + String(valZ);
  s += "\tGyro:" + String(gyroAverage);
  Serial.println(s);
  
  

  long timePassed = millis() - lastConnectionTime;
  if(!client.connected() && timePassed > postingInterval) {
    Serial.println("--------------");
    httpRequest();
  }
  lastConnected = client.connected();



  delay(100);
}

void resetArduino() {
  Serial.println("Resetting");
  //digitalWrite(resetPin, LOW);
}

void getGyroValues () {
  byte MSB, LSB;

  MSB = readI2C(0x29);
  LSB = readI2C(0x28);
  x = ((MSB << 8) | LSB);

  MSB = readI2C(0x2B);
  LSB = readI2C(0x2A);
  y = ((MSB << 8) | LSB);

  MSB = readI2C(0x2D);
  LSB = readI2C(0x2C);
  z = ((MSB << 8) | LSB);
}

int readI2C (byte regAddr) {
  Wire.beginTransmission(Addr);
  Wire.write(regAddr);                // Register address to read
  Wire.endTransmission();             // Terminate request
  Wire.requestFrom(Addr, 1);          // Read a byte
  while (!Wire.available()) { };      // Wait for receipt
  return (Wire.read());               // Get result
}

void writeI2C (byte regAddr, byte val) {
  Wire.beginTransmission(Addr);
  Wire.write(regAddr);
  Wire.write(val);
  Wire.endTransmission();
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
    String s = "GET /340HarvardSt/?v=0.2";
    s += "&gyro=" + String(gyroAverage);
    s += "&valX=" + String(valX);
    s += "&valY=" + String(valY);
    s += "&valZ=" + String(valZ);
    s += " HTTP/1.1";
    Serial.println(s);
    client.println(s);
    client.println("Host: jimmybyrum.com");
    client.println("User-Agent: arduino-pro-mini");
    client.println("Connection: close");
    client.println();
    digitalWrite(statusLed, HIGH);
  } else {
    hasEthernet = false;
    Serial.println("FAILED");
    digitalWrite(statusLed, LOW);
  }
  client.stop();
  Serial.println("--------------");
  lastConnectionTime = millis();
}
