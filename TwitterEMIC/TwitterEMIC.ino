// Included libraries
#include <SoftwareSerial.h> 
#include <LiquidCrystal.h>
#include <Bridge.h>
#include <Temboo.h>

// --------------------------------------------------------------------------------------------------------
// Things you need to edit...
// --------------------------------------------------------------------------------------------------------
#include "TembooAccount.h" // contains Temboo account information *** YOU WILL NEED TO EDIT THIS FILE ***

/*** SUBSTITUTE YOUR VALUES BELOW: ***/
// Note that for additional security and reusability, you could
// use #define statements to specify these values in a .h file.
const String TWITTER_ACCESS_TOKEN = "your_gibberish_twitter_access_token";
const String TWITTER_ACCESS_TOKEN_SECRET = "your_gibberish_twitter_access_secret";
const String TWITTER_API_KEY = "your_gibberish_twitter_api_key";
const String TWITTER_API_SECRET = "your_gibberish_twitter_api_secret";
// --------------------------------------------------------------------------------------------------------

// Only the following can be used for RX: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

// EMIC2 PINS
#define EMICrxPin 10    // EMIC2 SOUT 
#define EMICtxPin 11    // EMIC2 SIN  

// LCD PINS
#define LCDRSPin  9  // RS
#define LCDRWPin  8  // E

#define LCDD4Pin  7  // D4
#define LCDD5Pin  6   // D5
#define LCDD6Pin  5   // D6
#define LCDD7Pin  4   // D7

// Global Variables
bool firstRun = true;

// Setup our LCD
LiquidCrystal lcd(LCDRSPin, LCDRWPin, LCDD4Pin, LCDD5Pin, LCDD6Pin, LCDD7Pin); // rs, rw, d4, d5, d6, d7, d8

// Setup our Softare Serial for the EMIC2
SoftwareSerial emic =  SoftwareSerial(EMICrxPin, EMICtxPin);

void setup()  
{
  // Setup Logging
  Serial.begin(9600); // Serial for feedback
  Serial.println(F("Start"));

  // Setup LCD
  lcd.begin(2, 16); // 2 lines x 16 chars  
  // Update user!
  lcdTextTop(F("Warm up"));
  
  // Setup EMIC
  pinMode(EMICtxPin, OUTPUT);
  pinMode(EMICrxPin, INPUT);

  // Setup logging
  emic.begin(9600); // SoftwareSerial for EMIC

  // Wait until ready
  waitReady();
  
  // Initiate Twitter feed
  Bridge.begin();  
  
  // Update user!
  lcdTextBottom(F("Ready."));
  
  // Setup EMIC
  maxVolume(); // Set our volume on the EMIC
  setVoice(0);  // Set our voice on the EMIC
}

void loop()  // Main code, to run repeatedly
{
  Serial.println(F("Go"));
  lcdClear();
  lcdTextTop(F("Checking"));
  lcdTextBottom(F("Twitter"));
  
  // Start our check on Twitter
  TembooChoreo HomeTimelineChoreo;
  HomeTimelineChoreo.begin();    // Invoke the Temboo client.
    
  // set Temboo account credentials
  HomeTimelineChoreo.setAccountName(TEMBOO_ACCOUNT);
  HomeTimelineChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  HomeTimelineChoreo.setAppKey(TEMBOO_APP_KEY);
    
  // tell the Temboo client which Choreo to run (Twitter > Timelines > HomeTimeline)
  HomeTimelineChoreo.setChoreo("/Library/Twitter/Timelines/HomeTimeline");
        
  // set the required choreo inputs, see https://www.temboo.com/library/Library/Twitter/Timelines/HomeTimeline/
  HomeTimelineChoreo.addInput(F("Count"),               F("1"));                  //String(TweetsMax)); // the max number of Tweets to return from each request
  HomeTimelineChoreo.addInput(F("AccessToken"),         TWITTER_ACCESS_TOKEN);
  HomeTimelineChoreo.addInput(F("AccessTokenSecret"),   TWITTER_ACCESS_TOKEN_SECRET);
  HomeTimelineChoreo.addInput(F("ConsumerKey"),         TWITTER_API_KEY);    
  HomeTimelineChoreo.addInput(F("ConsumerSecret"),      TWITTER_API_SECRET);

  // next, we'll define two output filters that let us specify the elements of the response from Twitter that we want to receive see the examples at http://www.temboo.com/arduino
  HomeTimelineChoreo.addOutputFilter(F("tweet"), F("/[1]/text"), F("Response"));  
  // and the name of the author 
  HomeTimelineChoreo.addOutputFilter(F("author"), F("/[1]/user/screen_name"), F("Response"));
    
  //for (int i=1;i<=TweetsMax;i++) {
  //  //HomeTimelineChoreo.addOutputFilter(String(i+(outer*8)), "/["+String(i+(outer*8))+"]/user/screen_name", "Response");
  //  HomeTimelineChoreo.addOutputFilter("tweet", "/[" + String(i) + "]/text", "Response");    
  //  HomeTimelineChoreo.addOutputFilter("author", "/[" + String(i) + "]/user/screen_name", "Response");    
  //}
    
  unsigned int returnCode = HomeTimelineChoreo.run();
    
  if(returnCode == 0) { // a response code of 0 means success; print the API response
            
    String author; // a String to hold the tweet author's name
    String tweet; // a String to hold the text of the tweet

    while(HomeTimelineChoreo.available()) {
        
      // read the name of the output item
      String nam = HomeTimelineChoreo.readStringUntil('\x1F');
      nam.trim();

      // read the value of the output item
      String data = HomeTimelineChoreo.readStringUntil('\x1E');
      data.trim();
      Serial.println("N:" + nam + " D:" + data);

      // assign the value to the appropriate String
      if (nam == "tweet") {
        tweet = data;
      } else if (nam == "author") {
        author = data;
      }
    }
      // Serial.println("@" + author + " - " + tweet);
      // We have the Author and Tweet.  Lets send it to our talk box!!!
      
    if (author != "") {
      lcdClear();
      // Speak the name first...
      doText("From " + author);
      delay(250);
      // Speak the text
      lcdClear();
      doText("Said " + tweet);
      delay(250);
    }
      else
    {        
      lcdClear();
      lcdTextTop(F("Empty"));
    }
  } 
  else 
  {
    // there was an error
    String inp;
    while(HomeTimelineChoreo.available()) {
      char c = HomeTimelineChoreo.read();
      //inp = inp + c;
      Serial.print(c);
    }
    Serial.println("");
    if (returnCode == 6) {
      doText(F("No Internet"));      
    }
    else 
    {
      doText(F("Error "));
      lcdTextBottom(String(returnCode));
    }
  }

  HomeTimelineChoreo.close();
 
  if (firstRun == true) {
    firstRun = false;
    sing();
  }
  else
  {
    Serial.print(F("[x"));
    delay(2500);
    Serial.print(F("x"));
    delay(2500);
    Serial.print(F("x"));
    delay(2500);
    Serial.print(F("x"));
    delay(2500);
    Serial.print(F("x"));
    delay(2500);
    Serial.println(F("x]"));
    delay(2500);
  } 
    
  lcdClear();

  // Additional tests you can run to test the LCD and EMIC2:  
  // voiceTest();
  // sing();  
}

void lcdTextTop(String inp) {
//  Serial.print(F("Top LCD: "));
  //Serial.println(inp);
  lcd.setCursor(0,0);
  lcd.print(inp);
}

void lcdTextBottom(String inp) {
//  Serial.print(F("Bottom LCD: "));
  //Serial.println(inp);
  lcd.setCursor(0,1);
  lcd.print(inp);
}

void lcdClear() {
//   Serial.println(F("Clear."));
   lcd.clear();
}

void voiceTest() {
  for (int i = 0; i < 8; i++) {
    lcdClear();
    lcdTextTop(F("Voice "));
    lcdTextBottom(String(i));
    setVoice(i);
    doText(F("EXTERMINATE"));
    delay(100);
  }
}

void setVoice(int voice) {
  // 0 to 8, 0 Perfect Paul, 1 Huge Harry, 2 Beautiful Betty, 3 Uppity Ursula, 4 Doc Dennis, 5 Kit the Kid, 6 Frail the Frank, 7 Rough Rita, 8 Wispering Wendy

  // Set some hard limits
  if (voice < 0) { voice = 0; }
  if (voice > 8) { voice = 8; } 
  
//  Serial.print(F("Voice@"));
//  Serial.println(voice);

  // Set voice EMIC2  
  emic.print('N'); // Tell it we are picking a voice
  emic.print(voice); // send the voice we picked
  emic.print('\n');  
  waitReply();
  
}

void waitReady() {
//  Serial.println(F("Getting ready."));
  emic.print('\n');             // Check if system online
  waitReply();
  //Serial.println(F("Ready."));
}

void maxVolume() {
//  Serial.println(F("Vol max."));
  // Set Volume EMIC2
  emic.print('V18'); // Range V-48 to V18
  emic.print('\n');  
  waitReply();
}

void doText(String inp) {  
  
  // Clean up the text
  inp.trim(); // Removes spaces
  lcdClear(); // Clears our LCD  
  
  // Does it fit all on one line?
  if (inp.length() > 15) { 
    
    // Show first 16 chars
    String topText = inp.substring(0,15);
    topText.trim();
    lcdTextTop(topText);
    
    // Show remaining chars
    String botText;
    
    if (inp.length() > 31) {
      botText = inp.substring(15,31);
    }
    else
    {
      botText = inp.substring(15,inp.length());
    }    
    botText.trim();
    lcdTextBottom(botText);
  }
  else
  {
    // 16 Characters or less, we only need to show the top line
    lcdTextTop(inp);
  }
  
  // EMIC2
  // Cleanup the text again for EMIC
  inp.replace(F("http://"), F("site "));
  inp.replace(F("@"), F("user "));
  inp.replace(F("#"), F("hashtag "));
  inp.replace(F("RT "), F("re tweet "));
  inp.replace(F("\""), F("quote "));
  inp.replace(F(":"),F(" "));
  inp.replace(F("â"),F(" "));
  
  emic.print('S');
  emic.print(inp); // send text
  emic.print('\n');  
  waitReply();
}

void sing(){
//  Serial.println(F("Singing."));
  // LCD  
  lcdClear();
  lcdTextTop(F("Daisy..."));
  // EMIC
  emic.print("D1\n");
  waitReply();
}

void waitReply(){
//  Serial.println(F("Waiting ready."));
  while (emic.read() != ':');   // Wait for : meanin ready
  delay(30);                   // Add delay
  emic.flush();                 // Flush the receive buffer
}
