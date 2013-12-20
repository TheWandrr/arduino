int sleepPin = 4; //Turning sleep high turns on the Accelerometer
int xpin= A0;
int ypin = A1;
int zpin = A2;

void setup() {
  Serial.begin(9600);
  
  pinMode(sleepPin,OUTPUT);
  digitalWrite(sleepPin, HIGH);//turns off sleep mode and activates device
  
  pinMode(xpin, INPUT);//input mode
  digitalWrite(xpin, HIGH);//turn on pull up resistor
  
  pinMode(ypin, INPUT);//input mode
  digitalWrite(ypin, HIGH);//turn on pull up resistor
  
  pinMode(zpin, INPUT);//input mode
  digitalWrite(zpin, HIGH);//turn on pull up resistor
}

void loop() {
  delay(2000); //Delay for readability
  Serial.print("X Reading: "); 
  Serial.println(analogRead(xpin), DEC);
  Serial.print("Y Reading: "); 
  Serial.println(analogRead(ypin), DEC);
  Serial.print("Z Reading: "); 
  Serial.println(analogRead(zpin), DEC);
}
