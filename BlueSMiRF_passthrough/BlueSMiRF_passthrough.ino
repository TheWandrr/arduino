/*
  Example Bluetooth Serial Passthrough Sketch
 by: Jim Lindblom
 SparkFun Electronics
 date: February 26, 2013
 license: Public domain

 This example sketch converts an RN-42 bluetooth module to
 communicate at 9600 bps (from 115200), and passes any serial
 data between Serial Monitor and bluetooth module.
 */
#include <SoftwareSerial.h>

int bluetoothTx = 0;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 1;  // RX-I pin of bluetooth mate, Arduino D3

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

int led = 13;

#define DEBUG_ENABLED 1

void setup() {
  pinMode(led, OUTPUT); 
  
  pinMode(bluetoothRx, INPUT);
  pinMode(bluetoothTx, OUTPUT);
  
  Serial.begin(9600);  // Begin the serial monitor at 9600bps
  //while( ! Serial) ;
  
  delay(1000);
  
  bluetooth.begin(115200);
//  bluetooth.print("$$$");
//  delay(100);
//  bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
//  bluetooth.begin(9600);  // Start bluetooth serial at 9600
//  delay(100);
  bluetooth.println("+INQ=1");
}

void loop() {
  if (bluetooth.available()) // If the bluetooth sent any characters
  {
    // Send any characters the bluetooth prints to the serial monitor
    char inBt = bluetooth.read();
    Serial.print(inBt);
    toggleLed(inBt);
  }
  if (Serial.available()) // If stuff was typed in the serial monitor
  {
    // Send any characters the Serial monitor prints to the bluetooth
    char inChar = Serial.read();
    bluetooth.print(inChar);
    toggleLed(inChar);
  }
  // and loop forever and ever!
}

void toggleLed(char inChar) {
  switch (inChar) {
    case '1':
      digitalWrite(13, HIGH);
    break;
    case '3':
      bluetooth.print("\rAV+\r");
      digitalWrite(13, HIGH);
      delay(1000);
      digitalWrite(13, LOW);
    break;
    case '2':
      digitalWrite(13, LOW);
    break;
    case '4':
      bluetooth.print("\rAV-\r");
      digitalWrite(13, HIGH);
      delay(1000);
      digitalWrite(13, LOW);
    break;
  }
}

