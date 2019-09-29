void setup() { 
  Serial.begin(115200);   
  delay(100);
}

void loop() {
  if (Serial.available()) {  
    char mode = Serial.read();  
    Serial.println(mode);
  } 
}
