#include <SoftwareSerial.h>
int bluetoothTx = 0;
int bluetoothRx = 1;
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);


#include <Servo.h> 
Servo myservo;
int servoPin = 9;
int servoVal = 0;


int led = 13;

#define DEBUG_ENABLED 1

void setup() {
  pinMode(led, OUTPUT); 
  
  pinMode(bluetoothRx, INPUT);
  pinMode(bluetoothTx, OUTPUT);
  
  Serial.begin(9600);  // Begin the serial monitor at 9600bps
  //while( ! Serial) ;
  
  delay(1000);
  
  myservo.attach(servoPin);
  
  bluetooth.begin(115200);
  bluetooth.print("$$$");
  delay(100);
  bluetooth.println("+INQ=1");
}

void loop() {
  if (bluetooth.available()) {
    char inBt = bluetooth.read();
    process(inBt);
  }
  if (Serial.available()) {
    char inChar = Serial.read();
    process(inChar);
  }
}

void setServo(int value) {
  servoVal = value;
  myservo.write(servoVal);
}

void setServo(String command) {
  if (command == "open") {
    setServo(160);
  } else if (command == "close") {
    setServo(0);
  }
}

void process(char inChar) {
  switch (inChar) {
    case '1':
      digitalWrite(13, HIGH);
      setServo("open");
    break;
    case '0':
    case '2':
      digitalWrite(13, LOW);
      setServo("close");
    break;
  }
}

