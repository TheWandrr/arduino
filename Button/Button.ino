#include <Button.h>

void setup() {
  Serial.begin(9600);
  pinMode(5, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
}

void loop() {
  int btn5 = digitalRead(5);
  digitalWrite(13, btn5);
  digitalWrite(12, !btn5);
  
  int btn3 = digitalRead(3);
  digitalWrite(11, btn3);
  digitalWrite(9, !btn3);
  
  boolean both = false;
  if (btn5 == HIGH && btn3 == HIGH) {
    both = true;
  }
  int bothOn = LOW;
  if (both) {
    bothOn = HIGH;
  }
  digitalWrite(8, bothOn);
  digitalWrite(7, !bothOn);
  digitalWrite(6, bothOn);
  
  delay(50);
}

