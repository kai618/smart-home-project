void setup() {
  Serial.begin(115200);
  delay(100);
}

void loop() {
  char mode = '9';
  Serial.println(mode);
  delay(500);
}
