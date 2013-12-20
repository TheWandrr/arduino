#include <SPI.h>
#include <Ethernet.h>
#include "Accelerometer.h"
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
int gyroX, gyroY, gyroZ;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

Accelerometer myAccelerometer = Accelerometer();

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

int photoRPin = 3;
int minLight;
int maxLight;
int lightLevel;
int adjustedLightLevel;

const int numLightReadings = 10;
float lightReadings[numLightReadings];
int lightIndex = 0;
int lightTotal = 0;
int lightAverage = 0;

boolean isFree = false;

const int numReadings = 50;
float readings[numReadings];
int index = 0;
int total = 0;
int average = 0;

void setup() {
  Serial.begin(9600);
  
  Wire.begin();
  writeI2C(CTRL_REG1, 0x1F);    // Turn on all axes, disable power down
  writeI2C(CTRL_REG3, 0x08);    // Enable control ready signal
  writeI2C(CTRL_REG4, 0x80);    // Set scale (500 deg/sec)
  delay(100);                   // Wait to synchronize 
  
  lightLevel = analogRead(photoRPin);
  minLight = lightLevel-20;
  maxLight = lightLevel;
  
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
  
  //Connect up the following pins and your power rail
  //                    SL GS 0G   X  Y  Z
  myAccelerometer.begin(1, 4, 5, A0, A1, A2);
  
  delay(2000);
  
  myAccelerometer.calibrate();
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  
  matrix.begin(0x70);
  clearDisplay();
  
  for (int thisLightReading = 0; thisLightReading < numLightReadings; thisLightReading++) {
    lightReadings[thisLightReading] = 0;
  }
  
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop() {
  
  
  
  
  lightLevel=analogRead(photoRPin);
  if (minLight > lightLevel) {
    minLight=lightLevel;
  }
  if (maxLight < lightLevel) {
    maxLight=lightLevel;
  }
  
  lightTotal = lightTotal - lightReadings[lightIndex];
  lightReadings[lightIndex] = lightLevel;
  lightTotal = lightTotal + lightReadings[lightIndex];
  
  lightIndex = lightIndex + 1;
  if (lightIndex >= numLightReadings) {
    lightIndex = 0;
  }
  
  lightAverage = abs(lightTotal / numLightReadings);
  
  int adjustedBrightness = map(lightAverage, minLight, maxLight, 0, 15);
  Serial.print("Light: ");
  Serial.print(adjustedBrightness);
  Serial.print(", ");
  matrix.setBrightness(15 - adjustedBrightness);
  
  
  
  
  
  
  getGyroValues(); // Get new values
  // In following Dividing by 114 reduces noise
  Serial.println("GRYRO");
  Serial.print("Raw X:");  Serial.print(gyroX / 114);  
  Serial.print(" Raw Y:"); Serial.print(gyroY / 114);
  Serial.print(" Raw Z:"); Serial.println(gyroZ / 114);
  Serial.println("");
  
  
  
  
  
  
  myAccelerometer.read();
  float x = myAccelerometer._Xgs;
  float y = myAccelerometer._Ygs;
  float z = myAccelerometer._Xgs;
  
  Serial.print("X, Y, Z: ");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(", ");
  Serial.print(z);
  
  Serial.print(", ");
  
  
  
  
  
  
  
  
  unsigned int uS = sonar.ping();
  Serial.print("Ping: ");
  long cm = uS / US_ROUNDTRIP_CM;
  Serial.print(cm);
  Serial.println("cm");
  
  total = total - readings[index];
  float newReading = (x*100) + (y*100) + (z*100);
  readings[index] = newReading;
  total = total + readings[index];
  
  index = index + 1;
  if (index >= numReadings) {
    index = 0;
  }
  
  average = abs(total / numReadings);
  Serial.println(average);
  
  
  
  
  
  
  
  
  if (average < 10) {
    matrix.print(10000, DEC);
    matrix.writeDigitNum(1, average);
  } else {
    matrix.writeDigitNum(0, (average / 10) % 10);
    matrix.writeDigitNum(1, average % 10);
  }
  
  if (cm < 10) {
    matrix.drawColon(true);
  } else {
    matrix.drawColon(false);
  }
  
  matrix.writeDigitNum(3, (cm / 10) % 10);
  matrix.writeDigitNum(4, cm % 10);
  matrix.writeDisplay();
  
  
  
  
  
  
  if (average > 5) {
    isFree = false;
  } else {
    isFree = true;
  }
  
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
  gyroX = ((MSB << 8) | LSB);

  MSB = readI2C(0x2B);
  LSB = readI2C(0x2A);
  gyroY = ((MSB << 8) | LSB);

  MSB = readI2C(0x2D);
  LSB = readI2C(0x2C);
  gyroZ = ((MSB << 8) | LSB);
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


