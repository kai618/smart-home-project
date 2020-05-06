#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Chrono.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include <ESP8266HTTPClient.h>
#include <FS.h>

// ---------------------------------- Wifi
String ssid = "";
String pw = "";

bool linked = false;
Chrono wifiCheckTime;
bool tryingHomeWiFi = true;

// ---------------------------------- Firebase
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
unsigned long timeOut = 1000;
Chrono modeTimer;

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
  readData();
  waitForRetriever();
  beginWiFi(ssid, pw);
  SetUpLCD();
  dht.begin();
  setPin();
}

void loop() {
  checkWiFi();
  checkButtons();
  observeTempHumid();
  observeArduinoMode();
  setLightStatus();
}

// ----------------------------------

void setPin() {
  pinMode(light, OUTPUT);
  pinMode(indicator, OUTPUT);
  pinMode(btn, INPUT_PULLUP);
  pinMode(macbtn, INPUT_PULLUP);
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
      lcd.setCursor(1, 0);
      lcd.print("Temp :       C");
      lcd.setCursor(13, 0);
      lcd.print(char(223));
      lcd.setCursor(1, 1);
      lcd.print("Humid:      %");
      hasLabel = true;
      isMacPrinted = false;
    }
    lcd.setCursor(8, 0);
    lcd.print(temp);
    lcd.setCursor(8, 1);
    lcd.print(humid);
  }
}

void printMAC() {
  if (!isMacPrinted) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("MAC");
    lcd.setCursor(7, 0);
    lcd.print(WiFi.macAddress().substring(0, 8));
    lcd.setCursor(7, 1);
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
  Serial.println(nowMode);
  if (Serial.available()) receiveMode();
  else checkConnectedUNO();
}

void receiveMode() {
  modeTimer.restart();
  if (newMode != nowMode) {
    digitalWrite(indicator, HIGH);
    nowMode = newMode;
    postArduinoMode(nowMode);
  }
}

void checkConnectedUNO() {
  if (nowMode != 'x') {
    if (modeTimer.hasPassed(timeOut)) {
      modeTimer.restart();
      nowMode = 'x';
      digitalWrite(indicator, LOW);
      postArduinoMode(nowMode);
    }
  }
}

// on off light
void setLightStatus() {
  checkBtn();
  if (isBtnPressed) {
    isBtnPressed = false;
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
  if (digitalRead(macbtn) == HIGH) pressTime = 0;
  if (digitalRead(macbtn) == LOW && pressTime == 0) pressTime = millis();
  if (pressTime != 0 && digitalRead(macbtn) == LOW && millis() - pressTime >= waitTime) butState = true;
  if (butState && digitalRead(macbtn) == HIGH) {
    pressTime = 0;
    showTempHumid = !showTempHumid;
    showTempHumid ? printTempHumid() : printMAC();
  }
}

void checkBtn() {
  if (digitalRead(btn) == LOW) btnState = true;
  if (btnState && digitalRead(btn) == HIGH) {
    btnState = false;
  }
}

void switchLight() {
  isOn ? digitalWrite(light, HIGH) : digitalWrite(light, LOW);
}

// ---------------------------------- Firebase interaction
void postTempHumid() {
  if (linked) {
    checkBtn();
    String strTemp = String(temp);
    String strHumid = String(humid);
    Firebase.setString(path + "/temp_humid", String(strTemp + " " + strHumid));
  }
}

void postArduinoMode(char nowMode) {
  if (linked) {
    checkBtn();
    String strMode = "";
    strMode += nowMode;
    if (strMode == "x") {
      Firebase.setString(path + "/mode", strMode);
      Firebase.setString(path + "/hasMotion", strMode);
    }
    else if (strMode >= "0" && strMode <= "4") {
      Firebase.setString(path + "/mode", strMode);
    }
    else if (strMode >= "5" && strMode <= "6") {
      strMode = strMode == "5" ? "0" : "1";
      Firebase.setString(path + "/hasMotion", strMode);
    }
  }
}

void postLightStatus() {
  if (linked) {
    Firebase.setBool(path + "/led", isOn);
  }
}

void getLightStatus() {
  if (linked) {
    bool val = Firebase.getBool(path + "/led");
    if (!Firebase.failed()) isOn = val;
    switchLight();
  }
}

void postMacAddress() {
  if (linked) {
    Firebase.setString(path + "/MAC", WiFi.macAddress());
  }
}

// ---------------------------------- WiFi interaction
void checkWiFi() {
  if (WiFi.status() == WL_DISCONNECTED) {
    shadowTheMoon();
  }
  if (!linked && wifiCheckTime.hasPassed(8000)) {
    //    Serial.println(WiFi.status());
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.disconnect(true);
      tryingHomeWiFi ? beginWiFi(ssid, pw) : beginWiFi(retrieverSSID, retrieverPw);
      Serial.println(tryingHomeWiFi ? "Trying Home: " + ssid + " " + pw : "Trying Phone");
      tryingHomeWiFi = !tryingHomeWiFi;
    }
    else if (WiFi.SSID() == retrieverSSID) {
      if (getNewSSID() && getNewPw()) {
        storeData();
        beginWiFi(ssid, pw);
        tryingHomeWiFi = !tryingHomeWiFi;
        Serial.println("Retriever: " + ssid + " " + pw);
      }
      else {
        Serial.println("No responses from Retriever");
      }
    }
    else if (WiFi.SSID() == ssid) {
      Firebase.begin(fbHost, fbAuth);
      lightTheMoon();
      path = "modules/" + WiFi.macAddress();
      postMacAddress();
      postArduinoMode(nowMode);
      postLightStatus();
      Serial.println("Home connected: " + ssid + " " + pw);
    }
    wifiCheckTime.restart();
  }
}

void waitForRetriever() {
  Serial.println("Waiting 10s for Retriever...");
  beginWiFi(retrieverSSID, retrieverPw);
  delay(10000);
  if (getNewSSID() && getNewPw()) Serial.println("Retriever: " + ssid + " " + pw);
}

bool getNewSSID() {
  HTTPClient http;
  http.begin(ssidUrl);
  int httpCode = http.GET();
  if (httpCode <= 0) return false;
  ssid = http.getString();
  return true;
}

bool getNewPw() {
  HTTPClient http;
  http.begin(pwUrl);
  int httpCode = http.GET();
  if (httpCode <= 0) return false;
  pw = http.getString();
  return true;
}

void beginWiFi(String ssid, String pw) {
  WiFi.begin((const char*)ssid.c_str(), (const char*)pw.c_str());
}

// ---------------------------------- SPIFFS interaction
void readData() {
  if (SPIFFS.begin()) {
    File ssidFile = SPIFFS.open("/ssid.text", "r");
    if (ssidFile) {
      for (int i = 0; i < ssidFile.size(); i++) ssid += (char)ssidFile.read();
    }
    ssidFile.close();

    File pwFile = SPIFFS.open("/pw.text", "r");
    if (pwFile) {
      for (int i = 0; i < pwFile.size(); i++) pw += (char)pwFile.read();
    }
    pwFile.close();

    SPIFFS.end();
    Serial.println("Stored WiFi: " + ssid + " " + pw);
  } else Serial.println("Failed to read data");
}

void storeData() {
  if (SPIFFS.begin()) {
    File ssidFile = SPIFFS.open("/ssid.text", "w+");
    if (ssidFile) ssidFile.print(ssid);
    ssidFile.close();

    File pwFile = SPIFFS.open("/pw.text", "w+");
    if (pwFile) pwFile.print(pw);
    pwFile.close();
    SPIFFS.end();
  } else Serial.println("Failed to store data");
}
