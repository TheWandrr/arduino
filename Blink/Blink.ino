int led = 13;
int heatPin = 7;

int tempPin = 2;
float temp = 0;
const int rNum = 50;
float readings[rNum];
int ri = 0;
float rTotal = 0;
float rAverage = 0;

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(heatPin, OUTPUT);
  heatOn();
  
  for (int i = 0; i < rNum; i++) {
    readings[i] = 0;
  }
}

void heatOff() {
  digitalWrite(heatPin, LOW);
}

void heatOn() {
  digitalWrite(heatPin, HIGH);
}

void loop() {
  String s = "";
  
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
  
  if (rAverage > 46) {
    heatOff();
  } else {
    heatOn();
  }
  
  Serial.println(s);
  delay(300);
}

