#include <math.h>
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
//#include <Servo.h>

#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_7segment matrix = Adafruit_7segment();

//Servo myServo;
//int servoPin = 13;
//int servoPos = 0;

int buzzPin = 13;

int redLed = 12;
int grnLed = 10;

int pirInput = 1;

int photoRPin = 0;
int minLight;
int maxLight;
int lightLevel;
int adjustedLightLevel;

int pirState = LOW;
int pirValue = 0;

int redPin = 6;
int greenPin = 5;
int bluePin = 9;

int potPin = 1;
int sensorValue;

int buttonPin = 7;
int buttonState;

int tiltPin = 4;
int tiltState;

int thermPin = 3;
int thermValue;

void setup() {
  Serial.begin(9600);
  
  pinMode(redLed, OUTPUT);
  pinMode(grnLed, OUTPUT);
  
  pinMode(pirInput, INPUT);
  
  lightLevel = analogRead(photoRPin);
  minLight = lightLevel-20;
  maxLight = lightLevel;
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  pinMode(buttonPin, INPUT);

  pinMode(tiltPin, INPUT);
  
  pinMode(buzzPin, OUTPUT);
  digitalWrite(buzzPin, LOW);
  
  //myServo.attach(servoPin);
  
  matrix.begin(0x70);
  clearDisplay();
}

void loop() {
  sensorValue = analogRead(potPin);
  //int servoPos = map(sensorValue, 0, 683, 0, 180);
  int brightness = map(sensorValue, 0, 683, 0, 255);
  
  int matrixBrightness = map(sensorValue, 0, 683, 0, 15);
  matrix.setBrightness(matrixBrightness);
  
  pirValue = digitalRead(pirInput);
  
  if (pirValue == HIGH) {
    if (pirState == LOW) {
      digitalWrite(redLed, HIGH);
      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH) {
      digitalWrite(redLed, LOW);
      pirState = LOW;
    }
  }
  
  lightLevel=analogRead(photoRPin);
  if (minLight > lightLevel) {
    minLight=lightLevel;
  }
  if (maxLight < lightLevel) {
    maxLight=lightLevel;
  }
  adjustedLightLevel = map(lightLevel, minLight, maxLight, 0, 100);
  
  if (adjustedLightLevel < 60) {
    digitalWrite(grnLed, HIGH);
    //servoUp();
  } else {
    digitalWrite(grnLed, LOW);
    //servoDown();
  }
  
  //myServo.write(servoPos);
  
  buttonState = digitalRead(buttonPin);
  
  if (buttonState == HIGH) {
    digitalWrite(buzzPin, HIGH);
    delay(300);
    digitalWrite(buzzPin, LOW);
  }
  
  tiltState = digitalRead(tiltPin);
  
  int thermRaw = analogRead(thermPin);
  thermValue = int(Thermister(thermRaw));
  
  int rgbBlue = 0;
  if (buttonState == HIGH) {
    rgbBlue = brightness;
  }
  
  if (pirState == HIGH && adjustedLightLevel < 50) {
    setColor(brightness, brightness, rgbBlue);
  } else if (pirState == HIGH && adjustedLightLevel > 50) {
    setColor(brightness, 0, rgbBlue);
  } else if (pirState == LOW && adjustedLightLevel < 50) {
    setColor(0, brightness, rgbBlue);
  } else if (pirState == LOW && adjustedLightLevel > 50) {
    setColor(0, 0, rgbBlue);
  }
  
  displayLevels();
  
//  Serial.println(adjustedLightLevel);
  
  delay(300);
}

void servoUp() {
  //for(servoPos = 0; servoPos < 180; servoPos += 1) {
    //myServo.write(servoPos);
  //}
}

void servoDown() {
  //for(servoPos = 180; servoPos > 0; servoPos -= 1) {
    //myServo.write(servoPos);
  //}
}

void displayLevels() {
  //int n1 = thermValue / 100;
  //int n2 = 10 - n1;
  //matrix.writeDigitNum(1, n1);
  //matrix.writeDigitNum(3, n2, true);
  matrix.writeDigitNum(1, -1);
  matrix.writeDigitNum(3, -1, true);
  
  int potValue = map(sensorValue, 0, 683, 0, 99);
  matrix.writeDigitNum(0, (potValue / 10) % 10, true);
  
  if (tiltState == HIGH) {
    matrix.drawColon(true);
  } else {
    matrix.drawColon(false);
  }
  
  int lightValue = map(adjustedLightLevel, 0, 100, 0, 99);
  matrix.writeDigitNum(4, (lightValue / 10) % 10);
  matrix.writeDisplay();
}



void clearDisplay() {
  matrix.print(10000, DEC);
  matrix.writeDisplay();
}

void setColor(int red, int green, int blue) {
  analogWrite(redPin, (255-red));
  analogWrite(greenPin, (255-green));
  analogWrite(bluePin, (255-blue));  
}

double Thermister(int RawADC) {
  long Resistance;
  double Temp;
  // Assuming a 10k Thermistor.  Calculation is actually: Resistance = (1024 * BalanceResistor/ADC) - BalanceResistor
  Resistance=((10240000/RawADC) - 10000);
  // Saving the Log(resistance) so not to calculate it 4 times later.
  Temp = log(Resistance);
  // Now it means both "Temporary" and "Temperature"
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  // Convert Kelvin to Celsius
  Temp = Temp - 273.15;

  Serial.print("ADC: "); Serial.print(RawADC); Serial.print("/1024");  // Print out RAW ADC Number
  Serial.print(", Resistance: "); Serial.print(Resistance); Serial.print("ohms");
  
  //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert to Fahrenheit
  return Temp;  // Return the Temperature
}


