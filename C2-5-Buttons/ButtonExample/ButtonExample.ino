// Declarations
int LEDPIN = 9;
int BTNPIN = 4;
int TGLPIN = 3;
int TRMPIN = 0;

void setup() {
  // Output
  pinMode(LEDPIN,OUTPUT);
  
  // Digital Input
  pinMode(BTNPIN, INPUT);
  pinMode(TGLPIN, INPUT);
  
  // Analog Input
  pinMode(TRMPIN, INPUT);

  // Enable Serial.  To read these debug notes press Tools > Serial Monitor and select 19200 on the bottom right!
  Serial.begin(19200);
  Serial.println("Starting.");
}

void loop() {
  // IS power on?
  int tglOn = digitalRead(TGLPIN);
  Serial.println("digitalRead Toggle : " + String(tglOn));

  int btnOn = digitalRead(BTNPIN);
  Serial.println("digitalRead Button : " + String(btnOn));

  if (tglOn == HIGH) {
    // Power is on
    Serial.println("Toggle: On");

    if (btnOn == HIGH) {
      Serial.println("Button: On");
      // Power is on, Button is on
      int bright = analogRead(TRMPIN) / 4;
      analogWrite(LEDPIN,bright); // Send the 0-255 to LED that we have on Trimpot
      Serial.println("LED:  " + String(bright));
    }
    else
    {
      // Power is on, but button is off
      analogWrite(LEDPIN,0);
      Serial.println("Button: Off");
      Serial.println("LED:  Off");
    }    
  }
  else
  {
      analogWrite(LEDPIN,0);
      Serial.println("Toggle: Off");
      Serial.println("LED:  Off");
    // Power is off
  }
  delay(100);
}
