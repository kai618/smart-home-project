#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

String ssidUrl = "http://192.168.43.1:8080/ssid";
String pwUrl = "http://192.168.43.1:8080/password";

void setup() {
  Serial.begin(115200);
  connectWiFi();
}

void loop() {
  Serial.println("Retriever: " + getNewSSID() + " " + getNewPw());
  delay(3000);
}

void connectWiFi() {
  WiFi.begin("Sam", "");  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(500);
  }  
  turnOnBlueLED();
}

String getNewSSID() {
  HTTPClient http;
  http.begin(ssidUrl);
  int httpCode = http.GET();
  if (httpCode <= 0) return "404";
  return http.getString();
}

String getNewPw() {
  HTTPClient http;
  http.begin(pwUrl);
  int httpCode = http.GET();
  if (httpCode <= 0) return "404";
  return http.getString();
}

void turnOnBlueLED() {
  pinMode(D4, OUTPUT);
  digitalWrite(D4, LOW);
}
