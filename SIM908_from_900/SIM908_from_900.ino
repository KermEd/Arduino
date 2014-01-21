// Based on the 900 sim on cooking-hacks.com by Alejandro Gallego 
// Reworked from from Serial to Software Serial and reformulated for GPS by Lloyd Summers

#include <SoftwareSerial.h>

int txPin = 11;
int rxPin = 10;
int modGSMPin = 8;
int modGPSPin = 7;
int pinDelay = 3000;
int8_t answer;

char aux_str[30];
char phone_number[] = "4036900707";

SoftwareSerial myGSM(rxPin, txPin);

void setup() {
  
  // put your setup code here, to run once:
  pinMode(txPin, OUTPUT);
  pinMode(rxPin, INPUT); // used to turn on and off GSM
  pinMode(modGSMPin, OUTPUT);
  pinMode(modGPSPin, OUTPUT);

  Serial.begin(115200);
  Serial.println("Initializing...");

  myGSM.begin(9600);
  powerOnGSM();

  Serial.println("Connecting..."); // checks for connection I believe
  while ((sendATcommand("AT+CREG?", "+CREG: 0,1", 500) ||
          sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0);          
  // 0 = not registered, not searching
  // 1 = registered on home network
  // 2 = not registered, but searching
  // 3 = registration denied
  // 4 = unknown (out of coverage?)
  // 5 = roaming, registered
  // 6 = SMS only, home network
  // 7 = SMS only, roaming
  // 8 = EMS
  // 9 = CSFB not preferred, home network
  // 10 = CSBFB not preferred, roaming  
  
  sendATcommand("AT+CREG=?", "OK",1000); // verifies network list
  sendATcommand("AT+GSN","OK",5000); // IEMI
  sendATcommand("ATX4","OK",5000);   // Listen for dialtone + busy signal when dialing
    
  delay(1000);
  
  sendSMS("This is a test","4036900707"); // Test SMS
  
  hangUp();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void hangUp() {
  sendATcommand("ATH", "OK", 3000);
}

void sendSMS(char *message, char *number) {
  Serial.println("Sending SMS...");
  sendATcommand("AT+CMGF=1", "OK", 1000); // prep SMS mode
  // sprintf(aux_str,"AT+CMGS=\"%S\"", number);  
  // answer = sendATcommand(aux_str,">", 2000);
  answer = sendATcommand("AT+CMGS=\"4036900707\"","OK",1000);
  
  if (answer == 1) {
    // myGSM.println(message);
    myGSM.println("Test message.");
    myGSM.write(0x1A);
    answer = sendATcommand("", "OK", 20000);
    if (answer == 1) {
      Serial.println("Sent.");
    } else {
      Serial.println("Error");
    }
  } else {
    Serial.print("Error ");
    Serial.println(answer, DEC);
  }
}

void dial(char *number) {
  Serial.println("Dialing phone number...");
  sprintf(aux_str, "ATD%s;", number);
  sendATcommand(aux_str, "OK", 10000); // dial
}

void powerOnGSM() {
  uint8_t answer = 0;

  // check if the module is started
  answer = sendATcommand("AT", "OK", 5000);
  
  if (answer == 0) {
    // Send power on pulse
    digitalWrite(modGPSPin, LOW);
    digitalWrite(modGSMPin, HIGH);
    delay(pinDelay);
    //digitalWrite(modPin, LOW);

    // wait for answer
    while (answer == 0 ) {
      answer = sendATcommand("AT", "OK", 2000);
    }
  }
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout) {
  uint8_t x = 0, answer = 0;
  char response[100];
  unsigned long previous;
  memset(response, '\0', 100); // initalize string
  delay(100);

  while (myGSM.available() > 0) {
    myGSM.read(); // clears the buffer
  }

  myGSM.println(ATcommand);
  // Serial.println(ATcommand);

  x = 0;
  previous = millis();

  do {
    if (myGSM.available() != 0) {
      response[x] = myGSM.read();
      x++;
      if (strstr(response, expected_answer) != NULL) {
        answer = 1;
      }
    }
  } while ((answer == 0) && ((millis() - previous) < timeout));

  Serial.println(response);
  return answer;
}
