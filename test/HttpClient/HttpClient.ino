#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
 
const char* ssid = "Sam";
const char* password = "";
 
void setup(){ 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting..");
  }
}
 
void loop(){
  if (WiFi.status() == WL_CONNECTED){ //Check WiFi connection status   
    HTTPClient http; //Declare an object of class HTTPClient
     
    http.begin("http://192.168.43.1:12345/password.txt");  //Specify request destination
    int httpCode = http.GET(); //Send the request
     
    if (httpCode > 0) { //Check the returning code     
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload); //Print the response payload     
    }
    
    http.end(); //Close connection
    
  }
  delay(10000); //Send a request every 15 seconds 
}
