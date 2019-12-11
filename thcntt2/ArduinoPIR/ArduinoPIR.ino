int ledPin = 2;
int inputPin = 3;
int pirState = LOW;
int val = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  val = digitalRead(inputPin);

  if (val == HIGH) {
    digitalWrite(ledPin, HIGH);
    if (pirState == LOW) {
      Serial.println(1);
      pirState = HIGH;
    }
  } else {
    digitalWrite(ledPin, LOW);
    if (pirState == HIGH) {
      Serial.println(0);
      pirState = LOW;
    }
  }
}
