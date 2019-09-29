#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Chrono.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// ---------------------------------- Wifi Firebase
#define ssid "Kai" // laptop hot spot
#define password "1234567899"
#define fbHost "fb-demo-a57e3.firebaseio.com"
#define fbAuth "hdOHt0DQuTc6oRrVcxQe0Y2SujUDnmO9rjwUVTcM"
bool fbLinked = false;
String path = "";

// ---------------------------------- Components
#include "DHT.h"
#define DHTPIN D3 
#define DHTTYPE DHT21
DHT dht(DHTPIN, DHTTYPE);

// AM2301
float temp = 0;
float humid = 0;
int interval = 3000;
Chrono tempHumidSensor;

// Arduino mode
char nowMode = 'x';
unsigned long timeOut = 2000;
Chrono modeReceipt;

// Light 
#define indicator D8

#define light D2
bool isOn = false;

// Light button
#define btn D1
bool btnState = false;
bool isBtnPressed = false;

// LCD I2C
#define scl D6
#define sda D7
LiquidCrystal_I2C lcd(0x3F, 16, 2);
bool hasLabel = false;
bool showTempHumid = true;
bool isMacPrinted = false;

// Mac address Btn
#define macbtn D5
bool butState = false;  //true = pressed, false = unpressed
unsigned long pressTime = 0;
const unsigned long waitTime = 1000;

// ---------------------------------- main
void setup() {
  Serial.begin(115200);
  lightTheSun();
  WiFi.begin(ssid, password);
  
  SetUpLCD();
  dht.begin();
  pinMode(light, OUTPUT);
  pinMode(indicator, OUTPUT);
  pinMode(btn, INPUT_PULLUP);
  pinMode(macbtn, INPUT_PULLUP);
}
 
void loop() {  
  checkWiFi();
  checkButtons();
  observeTempHumid();
  observeArduinoMode();
  setLightStatus();
}

// ----------------------------------
void checkWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
//  Serial.println(WiFi.localIP());  
    if (!fbLinked) {
      Firebase.begin(fbHost, fbAuth);
      fbLinked = true;
      lightTheMoon();
      path = "modules/" + WiFi.macAddress();
      postMacAddress();
      postArduinoMode();
      postLightStatus();
    }
  }
  else {
    fbLinked = false;
    shadowTheMoon();
  }
}

void SetUpLCD() {
  Wire.begin(sda, scl);
  lcd.begin();
  lcd.home(); 
  lcd.print("  Starting...");
}

void observeTempHumid() {
  if (tempHumidSensor.hasPassed(interval)) {
    tempHumidSensor.restart();
    getLightStatus();
    temp = dht.readTemperature();
    humid = dht.readHumidity();
    printTempHumid();
    postTempHumid();
//    Serial.print("T: ");
//    Serial.print(temp);
//    Serial.print("   H: ");
//    Serial.println(humid);
  }
}

void printTempHumid() {
  if (showTempHumid) {
    if (!hasLabel) {
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Temp :       C");
      lcd.setCursor(13,0);
      lcd.print(char(223));
      lcd.setCursor(1,1);
      lcd.print("Humid:      %");
      hasLabel = true;
      isMacPrinted = false;
    }
    lcd.setCursor(8,0);
    lcd.print(temp);
    lcd.setCursor(8,1);
    lcd.print(humid);
  }
}

void printMAC() {
  if (!isMacPrinted) {    
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("MAC");
    lcd.setCursor(7,0);
    lcd.print(WiFi.macAddress().substring(0, 8));
    lcd.setCursor(7,1);
    lcd.print(WiFi.macAddress().substring(9));
    hasLabel = false;
    isMacPrinted = true;
  }  
}

// turn on built-in leds
void lightTheSun() {
  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);
}

void lightTheMoon() {
  pinMode(D4, OUTPUT);
  digitalWrite(D4, LOW);
}

void shadowTheMoon() {
  digitalWrite(D4, HIGH);
}

// get mode from FireGas arduino
void observeArduinoMode() {
  if (Serial.available()) receiveMode();
  else checkConnectedUNO();
}

void receiveMode() {
  char newMode = Serial.read();
    if (newMode == '1' || newMode == '2' || newMode == '3' || newMode == '0') {
      modeReceipt.restart();      
      if (newMode != nowMode) {
        digitalWrite(indicator, HIGH);
        nowMode = newMode;
        postArduinoMode();
      }
    }
}

void checkConnectedUNO() {
  if (nowMode != 'x') {
    if (modeReceipt.hasPassed(timeOut)) {
      modeReceipt.restart();
      nowMode = 'x';
      digitalWrite(indicator, LOW);
      postArduinoMode();
//      Serial.println("FireGas Arduino disconnected"); 
    }
  }
}

// on off light
void setLightStatus() {  
  checkBtn();
  if(isBtnPressed) {
    isBtnPressed = false;
    isOn = !isOn;
    switchLight();
    postLightStatus();
  }
}

// check buttons
void checkButtons() {
  checkBtn();
  checkMacBtnHeld();
}

void checkMacBtnHeld() {
  if(digitalRead(macbtn) == HIGH) pressTime = 0;  
  if(digitalRead(macbtn) == LOW && pressTime == 0) pressTime = millis();
  if(pressTime != 0 && digitalRead(macbtn) == LOW && millis() - pressTime >= waitTime) butState = true;
  if(butState && digitalRead(macbtn) == HIGH) {
    butState = false;
    pressTime = 0;
    showTempHumid = !showTempHumid;
    showTempHumid ? printTempHumid() : printMAC();
  }
}

void checkBtn() {
  if(digitalRead(btn) == LOW) btnState = true;
  if(btnState && digitalRead(btn) == HIGH) {
    btnState = false;
    isBtnPressed = true;
  }
}

void switchLight() {
  isOn ? digitalWrite(light, HIGH) : digitalWrite(light, LOW);
}

// ---------------------------------- Firebase interaction
void postTempHumid() {
  if (fbLinked) {
    checkBtn();
    String strTemp = String(temp);
    String strHumid = String(humid);
    Firebase.setString(path + "/temp_humid", String(strTemp + " " + strHumid));
  }
}

void postArduinoMode() {
  if (fbLinked) {    
    checkBtn();
    String strMode = "";
    strMode += nowMode;
    Firebase.setString(path + "/mode", strMode);
  }
}

void postLightStatus() {
  if (fbLinked) {
    checkBtn();
    Firebase.setBool(path + "/led", isOn);
  }
}

void getLightStatus() {
  if (fbLinked) {
    checkBtn();
    bool val = Firebase.getBool(path + "/led");
    if (!Firebase.failed()) isOn = val;
    switchLight();
  }
}

void postMacAddress() {
  if (fbLinked) {
    Firebase.setString(path + "/MAC", WiFi.macAddress());
  }
}
