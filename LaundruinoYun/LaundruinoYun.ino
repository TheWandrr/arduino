#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <HttpClient.h>
#include <Process.h>

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer yunServer;

unsigned long lastConnectionTime = 0;
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
int adjustedGyroAverage = 0;

#include <NewPing.h>
#define TRIGGER_PIN  6
#define ECHO_PIN     7
#define MAX_DISTANCE 200
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
long sonarDistance;

int knockPin = 0;
int knockReading = 0;
const int numKnockReadings = 50;
float knockReadings[numKnockReadings];
int knockIndex = 0;
int knockTotal = 0;
int knockAverage = 0;

int resetPin = 12;
int statusLed = 13;

void setup(){
//  digitalWrite(resetPin, HIGH);
  delay(200);
  
  Wire.begin();
  Bridge.begin();
  
//  pinMode(resetPin, OUTPUT);
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
  
  for (int i = 0; i < numKnockReadings; i++) {
    knockReadings[i] = 0;
  }
  
  yunServer.listenOnLocalhost();
  yunServer.begin();
  
  Serial.println("Ready");
}

void loop() {
  String s = "";
  
  YunClient yunClient = yunServer.accept();
  if (yunClient) {
    Serial.println("COMMAND RECEIVED");
    process(yunClient);
    yunClient.stop();
  }
  
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
  adjustedGyroAverage = map(gyroAverage, 0, 150, 0, 10);
  
  unsigned int uS = sonar.ping();
  sonarDistance = uS / US_ROUNDTRIP_CM;
  if (sonarDistance > 60) { sonarDistance = 60; }
  
  knockReading = analogRead(knockPin);
  knockTotal = knockTotal - knockReadings[knockIndex];
  knockReadings[knockIndex] = knockReading;
  knockTotal = knockTotal + knockReadings[knockIndex];
  knockIndex = knockIndex + 1;
  if (knockIndex >= numKnockReadings) {
    knockIndex = 0;
  }
  knockAverage = abs(knockTotal / numKnockReadings);
  
  s += "Knock: " + String(knockAverage);
  s += "\tSonar:" + String(sonarDistance);
  s += "\tX:" + String(valX);
  s += "\tY:" + String(valY);
  s += "\tZ:" + String(valZ);
  s += "\tGyro:" + String(gyroAverage);
  Serial.println(s);
  
  long timePassed = millis() - lastConnectionTime;
  if(timePassed > postingInterval) {
    Serial.println("--------------");
    httpRequest();
  }
  
  delay(100);
}

void resetArduino() {
  Serial.println("Resetting");
  digitalWrite(resetPin, LOW);
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
    while(!Wire.available()) { };       // Wait for receipt
    return(Wire.read());                // Get result
}

void writeI2C (byte regAddr, byte val) {
    Wire.beginTransmission(Addr);
    Wire.write(regAddr);
    Wire.write(val);
    Wire.endTransmission();
}




void httpRequest() {
  digitalWrite(statusLed, LOW);
  
//  Process wifiCheck;
//  wifiCheck.runShellCommand("/usr/bin/pretty-wifi-info.lua");
//  String wc = "";
//  while (wifiCheck.available() > 0) {
//    char _c = wifiCheck.read();
//    wc += _c;
//  }
//  Serial.println(wc);
  
  HttpClient httpClient;
  String u = "http://jimmybyrum.com/340HarvardSt/?v=0.1";
  u += "&gyro=" + String(gyroAverage);
  u += "&adjusted_gyro=" + String(adjustedGyroAverage);
  u += "&valX=" + String(valX);
  u += "&valY=" + String(valY);
  u += "&valZ=" + String(valZ);
  u += "&sonar=" + String(sonarDistance);
  u += "&knock=" + String(knockAverage);
  Serial.println(u);
  httpClient.get(u);
  
  String r = "";
  while (httpClient.available()) {
    char c = httpClient.read();
    r += c;
  }
  Serial.println(r);
  Serial.println("--------------");
  Serial.flush();
  
  digitalWrite(statusLed, HIGH);
  
  lastConnectionTime = millis();
}





void process(YunClient yunClient) {
  // read the command
  String command = yunClient.readStringUntil('/');
  
  if (command == "timer") {
    Serial.println("^^^^^^^^^^^^^^^^^");
    Serial.println("received timer");
    Serial.println("^^^^^^^^^^^^^^^^^");
  }

  // is "digital" command?
  if (command == "digital") {
    digitalCommand(yunClient);
  }

  // is "analog" command?
  if (command == "analog") {
    analogCommand(yunClient);
  }

  // is "mode" command?
  if (command == "mode") {
    modeCommand(yunClient);
  }
}

void digitalCommand(YunClient yunClient) {
  int pin, value;

  // Read pin number
  pin = yunClient.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (yunClient.read() == '/') {
    value = yunClient.parseInt();
    digitalWrite(pin, value);
  }
  else {
    value = digitalRead(pin);
  }

  // Send feedback to yunClient
  yunClient.print(F("Pin D"));
  yunClient.print(pin);
  yunClient.print(F(" set to "));
  yunClient.println(value);

  // Update datastore key with the current pin value
  String key = "D";
  key += pin;
  Bridge.put(key, String(value));
}

void analogCommand(YunClient yunClient) {
  int pin, value;

  // Read pin number
  pin = yunClient.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/analog/5/120"
  if (yunClient.read() == '/') {
    // Read value and execute command
    value = yunClient.parseInt();
    analogWrite(pin, value);

    // Send feedback to yunClient
    yunClient.print(F("Pin D"));
    yunClient.print(pin);
    yunClient.print(F(" set to analog "));
    yunClient.println(value);

    // Update datastore key with the current pin value
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
  }
  else {
    // Read analog pin
    value = analogRead(pin);

    // Send feedback to yunClient
    yunClient.print(F("Pin A"));
    yunClient.print(pin);
    yunClient.print(F(" reads analog "));
    yunClient.println(value);

    // Update datastore key with the current pin value
    String key = "A";
    key += pin;
    Bridge.put(key, String(value));
  }
}

void modeCommand(YunClient yunClient) {
  int pin;

  // Read pin number
  pin = yunClient.parseInt();

  // If the next character is not a '/' we have a malformed URL
  if (yunClient.read() != '/') {
    yunClient.println(F("error"));
    return;
  }

  String mode = yunClient.readStringUntil('\r');

  if (mode == "input") {
    pinMode(pin, INPUT);
    // Send feedback to yunClient
    yunClient.print(F("Pin D"));
    yunClient.print(pin);
    yunClient.print(F(" configured as INPUT!"));
    return;
  }

  if (mode == "output") {
    pinMode(pin, OUTPUT);
    // Send feedback to yunClient
    yunClient.print(F("Pin D"));
    yunClient.print(pin);
    yunClient.print(F(" configured as OUTPUT!"));
    return;
  }

  yunClient.print(F("error: invalid mode "));
  yunClient.print(mode);
}


