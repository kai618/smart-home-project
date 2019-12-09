#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  connectWiFi();

  server.on("/", respondRoot);
  server.on("/i", respondInfo);
  server.begin();
}

void loop() {
  server.handleClient();
}

void respondRoot() {
  server.send(200, "text/plain", "Hello world!");
}

void respondInfo() {
  server.send(200, "text/plain", "Hieu - 2170054");
}

void connectWiFi() {
  WiFi.begin("GHC_", "12356789");
  // WiFi.begin("HSU_Students", "dhhs12cnvch");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(500);
  }
  turnOnBlueLED();
  Serial.println(WiFi.localIP());
}

void startAP() {
  WiFi.softAP("ESP8266-AP", "12345678");
  printIP();
  turnOnBlueLED();
}

void turnOnBlueLED() {
  pinMode(D4, OUTPUT);
  digitalWrite(D4, LOW);
}

void printIP() {
  Serial.println();
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP: ");
  Serial.println(IP);
}
