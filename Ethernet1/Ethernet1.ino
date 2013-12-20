#include <SPI.h>
#include <Ethernet.h>
#include <NewPing.h>
#include <math.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

#define TRIGGER_PIN  12
#define ECHO_PIN     11
#define MAX_DISTANCE 200

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
int Addr = 105; // I2C address of gyro
int x, y, z;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xCB, 0x51 };
IPAddress ip(192,168,0,13);
//IPAddress myDns(1,1,1,1);

EthernetClient client;
char server[] = "jimmybyrum.com";

unsigned long lastConnectionTime = 0;
boolean lastConnected = false;
const unsigned long postingInterval = 5*1000;

int redLED = 9;
int grnLED = 8;

int busyLED = 7;
int freeLED = 6;

Adafruit_7segment matrix = Adafruit_7segment();

boolean isFree = false;

void setup() {
  Serial.begin(9600);
  
  Wire.begin();
  writeI2C(CTRL_REG1, 0x1F);    // Turn on all axes, disable power down
  writeI2C(CTRL_REG3, 0x08);    // Enable control ready signal
  writeI2C(CTRL_REG4, 0x80);    // Set scale (500 deg/sec)
  delay(100);                   // Wait to synchronize 
  
  pinMode(grnLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  
  pinMode(busyLED, OUTPUT);
  pinMode(freeLED, OUTPUT);
  
  blinkLEDs();
  
  if (Ethernet.begin(mac) == 0) {
    blinkLEDs();
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  
  delay(2000);
  
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  
  matrix.begin(0x70);
  clearDisplay();
}

void loop() {
  
  getGyroValues(); // Get new values
  // In following Dividing by 114 reduces noise
  Serial.println("GRYRO");
  Serial.print("Raw X:");  Serial.print(x / 114);  
  Serial.print(" Raw Y:"); Serial.print(y / 114);
  Serial.print(" Raw Z:"); Serial.println(z / 114);
  Serial.println("");

  
  
  unsigned int uS = sonar.ping();
  Serial.print("Ping: ");
  long cm = uS / US_ROUNDTRIP_CM;
  Serial.print(cm);
  Serial.println("cm");  

  
    
  if (cm < 10) {
    matrix.drawColon(true);
  } else {
    matrix.drawColon(false);
  }  
  matrix.writeDigitNum(3, (cm / 10) % 10);
  matrix.writeDigitNum(4, cm % 10);
  matrix.writeDisplay();

  
  
  if (isFree) {
    digitalWrite(busyLED, LOW);
    digitalWrite(freeLED, HIGH);
  } else {
    digitalWrite(busyLED, HIGH);
    digitalWrite(freeLED, LOW);
  }
  
  
  
  
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  long timePassed = millis() - lastConnectionTime;
  if(!client.connected() && timePassed > postingInterval) {
    Serial.println("-------------- PING --------------");
    httpRequest();
  }
  lastConnected = client.connected();


  
  delay(300);
}

void httpRequest() {
  digitalWrite(redLED, LOW);
  digitalWrite(grnLED, LOW);
  if (client.connect(server, 80)) {
    digitalWrite(grnLED, HIGH);
    if (isFree) {
      Serial.println("isFree: true. connecting...");
      client.println("GET /340HarvardSt/free.php HTTP/1.1");
    } else {
      Serial.println("isFree: false. connecting...");
      client.println("GET /340HarvardSt/busy.php HTTP/1.1");
    }
    client.println("Host: jimmybyrum.com");
    client.println("Connection: close");
    client.println();
    client.stop();
    digitalWrite(grnLED, LOW);
    
    lastConnectionTime = millis();
  } else {
    digitalWrite(redLED, HIGH);
    Serial.println("connection failed");
    client.stop();
    digitalWrite(redLED, LOW);
  }
}

void blinkLEDs() {
  digitalWrite(redLED, LOW);
  digitalWrite(grnLED, LOW);
  delay(1000);
  digitalWrite(redLED, HIGH);
  digitalWrite(grnLED, HIGH);
}

void clearDisplay() {
  matrix.print(10000, DEC);
  matrix.writeDisplay();
}

void getGyroValues () {
  byte MSB, LSB;

  Serial.println(1);
  MSB = readI2C(0x29);
  Serial.println(2);
  LSB = readI2C(0x28);
  Serial.println(3);
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
  while(!Wire.available()) { };       // Wait for receipt
  return(Wire.read());                // Get result
}

void writeI2C (byte regAddr, byte val) {
  Wire.beginTransmission(Addr);
  Wire.write(regAddr);
  Wire.write(val);
  Wire.endTransmission();
}


