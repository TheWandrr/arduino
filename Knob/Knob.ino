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
  myservo.write(servoVal);

  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (5.0 / 1023.0);
  s += "\tVoltage: " + String(voltage);
  
  Serial.println(s);
  
  delay(50);
}

void process(YunClient client) {
  client.println("Status: 200");
  client.println("Content-type: text/html");
  client.println();//mandatory blank line
  client.println("<!DOCTYPE html>");
  client.println("<html lang=\"en\"><head><title>Servo Control</title>");
  client.println("<link href=\"//netdna.bootstrapcdn.com/bootstrap/3.0.3/css/bootstrap.min.css\" rel=\"stylesheet\">");
  client.println("</head><body><div class=\"container\">");
  String command = client.readStringUntil('/');
  if (command == "servo") {
    servoCommand(client);
  }
  client.println("<a class=\"btn btn-success\" href=\"/arduino/servo/open\">Open</a>");
  client.println("<a class=\"btn btn-danger\" href=\"/arduino/servo/close\">Close</a>");
  client.println("</div></body></html>");
}

void setServo(int value) {
  servoVal = value;
}

void setServo(String command) {
  if (command == "open") {
    setServo(180);
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
    client.println("<p>Servo: " + command + "</p>");
  } else {
    if (value >= 0 && value <= 180) {
      setServo(value);
      client.println("<p>Servo set to " + String(value) + "</p>");
    } else {
      client.println("<p>Value must be between 0 and 180</p>");
    }
  }
}

