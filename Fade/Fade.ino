int led1 = 13;
int led2 = 10;

int brightness = 0;
int fadeAmount = 1;

// the setup routine runs once when you press reset:
void setup()  { 
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop()  { 
  analogWrite(led1, brightness);
  
  brightness = brightness + fadeAmount;

  if (brightness == 0 || brightness == 255) {
    fadeAmount = -fadeAmount;
  }
  
  if (brightness < 120) {
    analogWrite(led2, 255);
  } else {
    analogWrite(led2, 0);
  }
  
  delay(1);
}

