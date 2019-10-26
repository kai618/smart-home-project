#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Chrono.h>
#include <FS.h>
 
String retrieverSSID = "Sam";
String retrieverPw = "";
String ssidUrl = "http://192.168.43.1:8080/ssid";
String pwUrl = "http://192.168.43.1:8080/password";

String ssid = "Kai";
String pw = "1234567899";
 
void setup(){ 
  Serial.begin(115200);
  
  beginWiFi(ssid, pw);
}

bool linked = false;
Chrono wifiCheckTime;
bool tryingHomeWiFi = true;

void loop(){
  if(WiFi.status() == WL_DISCONNECTED) linked = false;
  if (!linked && wifiCheckTime.hasPassed(4000)) {
    if (WiFi.status() == WL_DISCONNECTED) {
      tryingHomeWiFi ? beginWiFi(ssid, pw) : beginWiFi(retrieverSSID, retrieverPw);
      tryingHomeWiFi = !tryingHomeWiFi;
//      Serial.println(tryingHomeWiFi ? "Trying Home: " + ssid + " " + pw : "Trying Phone"); 
    }
    else if (WiFi.status() == WL_CONNECTED && WiFi.SSID() == retrieverSSID) {
      if (getNewSSID() && getNewPw()) {        
        beginWiFi(ssid, pw);
        tryingHomeWiFi = !tryingHomeWiFi;
//        Serial.println("Retriever: " + ssid + " " + pw);
      }
      else {
//        Serial.println("No responses from Retriever");
      }
    }
    else if (WiFi.status() == WL_CONNECTED && WiFi.SSID() == ssid) {
      linked = true;
//      Serial.println("Home connected: " + ssid + " " + pw);
    }
    wifiCheckTime.restart();
  }    
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
