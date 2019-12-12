int ledPin = 2;
int inputPin = 3;
char nowMode = 'x';

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  observeMotion();
  Serial.println(nowMode);
}

void observeMotion() {
  if (digitalRead(inputPin) == HIGH) {
    digitalWrite(ledPin, HIGH);
    nowMode = '6';
  } else {
    digitalWrite(ledPin, LOW);
    nowMode = '5';
  }
}
