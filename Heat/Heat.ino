#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
Adafruit_7segment matrix = Adafruit_7segment();

int led = 13;
int heatPin = 7;

#include <math.h>
int tempPin = 0;
float temp = 0;
const int rNum = 50;
float readings[rNum];
int ri = 0;
float rTotal = 0;
float rAverage = 0;

boolean over = true;

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

//  temp = analogRead(tempPin);
  
//  float voltage = temp * 5.0;
//  voltage /= 1024.0;
//  s += "Volts: " + String(voltage);
  
//  float celcius = (voltage - 0.5) * 100;
  //float celcius = Thermister();
  
//  rTotal = rTotal - readings[ri];
//  readings[ri] = celcius;
//  rTotal = rTotal + readings[ri];
//  ri = ri + 1;
//  if (ri >= rNum) {
//    ri = 0;
//  }
//  rAverage = rTotal / rNum;

//  s += "\tTemp (C): " + String(rAverage);
  s += "\tMax (C): " + String(maxTemp);

//  int tempStr = int(rAverage);
//  matrix.writeDigitNum(0, (tempStr / 10) % 10);
//  matrix.writeDigitNum(1, tempStr % 10);
  matrix.writeDigitNum(0, 1);
  matrix.writeDigitNum(1, 1);
  matrix.drawColon(true);
  matrix.writeDigitNum(3, (maxTemp / 10) % 10);
  matrix.writeDigitNum(4, maxTemp % 10);
  matrix.writeDisplay();

//  if (rAverage > maxTemp) {
//    s += "\tHeat: ON";
//    over = true;
//    heatOff();
//  } else if (rAverage < (maxTemp - 2) && over) {
//    s += "\tHeat: OFF";
//    over = false;
//    heatOn();
//  }
  heatOn();
  
  Serial.println(s);
  delay(300);
}

double Thermister() {
  int RawADC = analogRead(tempPin);
  double Temp;
  Temp = log(((10240000 / RawADC) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
  Temp = Temp - 273.15;            // Convert Kelvin to Celcius
  //Temp = (Temp * 9.0) / 5.0 + 32.0; // Convert Celcius to Fahrenheit
  return Temp;
}

