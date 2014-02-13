#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
YunServer server;

#include <Servo.h> 
Servo myservo;
int servoPin = 9;
int servoVal = 0;

int led = 13;

void setup() {
  Serial.begin(9600);
  //while (!Serial) ;
  
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Bridge.begin();
  digitalWrite(led, HIGH);
  
  myservo.attach(servoPin);
  
  server.listenOnLocalhost();
  server.begin();
}

void loop() {
  String s = "";
  
  YunClient client = server.accept();
  if (client) {
    process(client);
    client.stop();
  }
  
  s += "Servo: " + String(servoVal);
  
  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (5.0 / 1023.0);
  s += "\tVoltage: " + String(voltage);
  
  Serial.println(s);
  
  delay(50);
}

void process(YunClient client) {
  String command = client.readStringUntil('/');
  if (command == "servo") {
    servoCommand(client);
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

void servoCommand(YunClient client) {
  YunClient c = client;
  int value = c.parseInt();
  String command = client.readString();
  command.trim();
  if (command == "open" || command == "close") {
    setServo(command);
    client.print("Servo: " + command);
  } else {
    if (value >= 0 && value <= 180) {
      setServo(value);
      client.print("Servo set to " + String(value));
    } else {
      client.print("Value must be between 0 and 180");
    }
  }
}

