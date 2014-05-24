
int LEDPin = 3; // what PIN are you using for your simple ON / OFF 2 prong LED?

void setup() {
  // Setup your pins here
  pinMode(LEDPin, OUTPUT);
}

void loop() {
  
  // Digital ON / OFF
  digitalWrite(LEDPin, LOW);
  delay(150);
  digitalWrite(LEDPin, HIGH);
  delay(150);

  // Faux Analog OFF > ON  
  for(int i=0; i< 255; i++) {
    analogWrite(LEDPin,i);
    delay(2);
  }
  
  // Faux Analog ON > OFF
  for(int i=0; i< 255; i++) {
    analogWrite(LEDPin,255-i);
    delay(2);
  }
  
}
