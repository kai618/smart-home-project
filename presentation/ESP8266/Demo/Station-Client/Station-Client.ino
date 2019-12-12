#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

void setup() {
  Serial.begin(115200);
  connectWiFi();
}

void loop() {
  Serial.println("Data: " + getData());
  delay(3000);
}

void connectWiFi() {
  WiFi.begin("HSU_Students", "dhhs12cnvch");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(500);
  }  
  turnOnBlueLED();
}

String getData() {
  HTTPClient http;
  http.begin("http://10.104.23.109:3000/");
  int httpCode = http.GET();
  if (httpCode <= 0) return "404";
  return http.getString();
}

void turnOnBlueLED() {
  pinMode(D4, OUTPUT);
  digitalWrite(D4, LOW);
}
