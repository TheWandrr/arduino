/*
  Blink
 Turns on an LED on for one second, then off for one second, repeatedly.

 This example code is in the public domain.
 */

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
int tempSensor = 0;
int vibePin = 10;

const int rNum = 50;
float readings[rNum];
int ri = 0;
float rTotal = 0;
float rAverage = 0;

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  pinMode(vibePin, OUTPUT);

  for (int i = 0; i < rNum; i++) {
    readings[i] = 0;
  }
}

// the loop routine runs over and over again forever:
void loop() {
  String s = "";

  int reading = analogRead(tempSensor);

  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0;
  s += "Volts: " + String(voltage);

  float temperatureC = (voltage - 0.5) * 100;
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;

  rTotal = rTotal - readings[ri];
  readings[ri] = temperatureC;
  rTotal = rTotal + readings[ri];
  ri = ri + 1;
  if (ri >= rNum) {
    ri = 0;
  }
  rAverage = rTotal / rNum;

  s += "\tTemp (C): " + String(rAverage);

  digitalWrite(led, HIGH);
  if (rAverage > 16.0) {
    digitalWrite(vibePin, HIGH);
  } else {
    digitalWrite(vibePin, LOW);
  }
  delay(100);
  digitalWrite(led, LOW);
  delay(10);

  Serial.println(s);
}

