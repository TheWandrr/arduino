#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
Adafruit_7segment matrix = Adafruit_7segment();

int led = 13;
int heatPin = 7;

int tempPin = 2;
float temp = 0;
const int rNum = 50;
float readings[rNum];
int ri = 0;
float rTotal = 0;
float rAverage = 0;

int potPin = 1;

int maxTemp = 50;

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(heatPin, OUTPUT);
  heatOn();
  
  for (int i = 0; i < rNum; i++) {
    readings[i] = 0;
  }
  
  matrix.begin(0x70);
}

void heatOff() {
  digitalWrite(heatPin, LOW);
}

void heatOn() {
  digitalWrite(heatPin, HIGH);
}

void setMaxTemp() {
  int temp = analogRead(potPin);
  int val = map(temp, 0, 1023, 0, 99);
  maxTemp = val;
}

void loop() {
  String s = "";
  
  setMaxTemp();
  
  temp = analogRead(tempPin);
  float voltage = temp * 5.0;
  voltage /= 1024.0;
  s += "Volts: " + String(voltage);
  
  float celcius = (voltage - 0.5) * 100;
  
  rTotal = rTotal - readings[ri];
  readings[ri] = celcius;
  rTotal = rTotal + readings[ri];
  ri = ri + 1;
  if (ri >= rNum) {
    ri = 0;
  }
  rAverage = rTotal / rNum;

  s += "\tTemp (C): " + String(rAverage);
  s += "\tMax (C): " + String(maxTemp);
  
  int tempStr = int(rAverage);
  matrix.writeDigitNum(0, (tempStr / 10) % 10);
  matrix.writeDigitNum(1, tempStr % 10);
  matrix.drawColon(true);
  matrix.writeDigitNum(3, (maxTemp / 10) % 10);
  matrix.writeDigitNum(4, maxTemp % 10);
  matrix.writeDisplay();
  
  if (rAverage > maxTemp) {
    heatOff();
  } else {
    heatOn();
  }
  
  Serial.println(s);
  delay(300);
}

