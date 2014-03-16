#include <Servo.h>

Servo myservo;
int servoPin = 9;
int pos = 0;
int max_pos = 160;
int step_by = 1;

void setup() {
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object
}


void loop() {
  for (pos = 0; pos < max_pos; pos += step_by) {
    myservo.write(pos);
    delay(15);
  }
}

