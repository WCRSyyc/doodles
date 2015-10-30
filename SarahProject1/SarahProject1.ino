int ledPin = 13;  
unsigned long ONE_SECOND = 1000;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin,HIGH);
  delay(ONE_SECOND); //1 second
  digitalWrite(ledPin, LOW);
  delay(1000);
}
