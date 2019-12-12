#include <Chrono.h>
#include <Servo.h>

// --------------------------------- Components

// pin input
#define servo 2
#define button 3
#define gasSen A1
#define flameSen A0

#define grnLed 5
#define bluLed 6
#define redLed 7
#define buzzer 4

Chrono alertGas;
bool alertStateGas = true;
unsigned long spacingGas = 300;

Chrono alertFire;
bool alertStateFire = true;
unsigned long spacingFire = 200;

// Threshold
const int gasLimit = 600;
const int flameLimit = 150;

// State
bool mainState = false;  //true = on, false = off
bool butState = false;  //true = pressed, false = unpressed
unsigned long pressTime = 0;
const unsigned long waitTime = 500;

// Servo

Servo sv;
int angle = 0;
int svStep = 1;

//Mode
const char OffMode = '0';
const char SafeMode = '1';
const char GasMode = '2';
const char FireMode = '3';

char nowMode = '0';

Chrono txTimer;

// --------------------------------- Main

void setup() {
  Serial.begin(115200);
  delay(100);

  setupServo();
  setupLEDpin();
  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT_PULLUP);
}

void loop() {
  checkButton();
  if (mainState) setOnMode();
  else setOffMode();

  changeOutputMode();
  sendMode();
}

// --------------------------------- Input Sensors buttons Servo

void sendMode() {
  if(txTimer.hasPassed(500)) {
    Serial.println(nowMode);
    txTimer.restart();
  }
}

void checkButton() {
  if (mainState == false) {
    if (isButtonPressed()) setSafeModeOutput();
  }
  else {
    if (isButtonHeld()) setOffModeOutput();
  }
}
bool isButtonPressed() {
  if (digitalRead(button) == LOW) butState = true;
  if (butState && digitalRead(button) == HIGH) {
    butState = false;
    return true;
  }
  return false;
}
bool isButtonHeld() {
  if (digitalRead(button) == HIGH) pressTime = 0;
  if (digitalRead(button) == LOW && pressTime == 0) pressTime = millis();
  if (pressTime != 0 && digitalRead(button) == LOW && millis() - pressTime >= waitTime) butState = true;
  if (butState && digitalRead(button) == HIGH) {
    butState = false;
    pressTime = 0;
    return true;
  }
  return false;
}

void setOnMode() {
  setDetectMode(chooseDetectMode());
}

void setOffMode() {
  if (nowMode != OffMode) {
//    Serial.println(OffMode);
    nowMode = OffMode;
  }
  getHandBack();
}

void setDetectMode(int i) {
  switch (i) {
    case 1: setSafeMode(); break;
    case 2: setGasMode(); break;
    case 3: setFireMode(); break;
  }
}

int chooseDetectMode() {
  if (detectFlame()) return 3;
  else if (detectGas()) return 2;
  else return 1;
}

void setSafeMode() {
  if (nowMode != SafeMode) {
//    Serial.println(SafeMode);
    nowMode = SafeMode;
  }
  scoutArea(30);
}

void setGasMode() {
  if (nowMode != GasMode) {
//    Serial.println(GasMode);
    nowMode = GasMode;
  }
  scoutArea(10);
}

void setFireMode() {
  if (nowMode != FireMode) {
//    Serial.println(FireMode);
    nowMode = FireMode;
  }
  delay(15);
}

void scoutArea(int slowness) {
  angle += svStep;
  sv.write(angle);
  if (angle < 0 || angle > 180) svStep = -svStep;
  delay(slowness);
}

void getHandBack() {
  if (angle > 0) {
    sv.write(angle--);
    delay(20);
  }
}

bool detectGas() {
  // Serial.println(analogRead(gasSen));
  if (analogRead(gasSen) > gasLimit) return true;
  return false;
}

bool detectFlame() {
  // Serial.println(analogRead(flameSen));
  if (analogRead(flameSen) < flameLimit) return true;
  return false;
}

void setupServo() {
  sv.attach(servo);
  angle = sv.read();
}

// --------------------------------- Output LED Buzzer

void changeOutputMode() {
  switch (nowMode) {
    case OffMode : setOffModeOutput(); break;
    case SafeMode : setSafeModeOutput(); break;
    case GasMode : setGasModeOutput(); break;
    case FireMode : setFireModeOutput(); break;
  }
}
void setOffModeOutput() {
  setColor(0, 0, 0);
  if (mainState) {
    mainState = false;
    beepOff();
  }
}
void setSafeModeOutput() {
  setColor(0, 255, 0);
  if (!mainState) {
    mainState = true;
    beepOn();
  }
}
void setGasModeOutput() {
  delay(15);
  if (alertGas.hasPassed(spacingGas)) {
    alertGas.restart();
    switchAlertGas();
  }
}
void setFireModeOutput() {
  delay(15);
  if (alertFire.hasPassed(spacingFire)) {
    alertFire.restart();
    switchAlertFire();
  }
}
void switchAlertGas() {
  if (alertStateGas) setAlertONGas();
  else setAlertOFF();
  alertStateGas = !alertStateGas;
}
void switchAlertFire() {
  if (alertStateFire) setAlertONFire();
  else setAlertOFF();
  alertStateFire = !alertStateFire;
}
void setAlertONGas() {
  setColor(255, 255, 0);
  tone(buzzer, 500, spacingGas);
}
void setAlertONFire() {
  setColor(255, 0, 0);
  tone(buzzer, 1000, spacingFire);
}
void setAlertOFF() {
  setColor(0, 0, 0);
}
void beepOn() {
  tone(buzzer, 700, 100);
}
void beepOff() {
  tone(buzzer, 200, 500);
}
void setColor(int redVal, int grnVal, int bluVal) {
  analogWrite(redLed, redVal);
  analogWrite(grnLed, grnVal);
  analogWrite(bluLed, bluVal);
}
void setupLEDpin() {
  pinMode(redLed, OUTPUT);
  pinMode(grnLed, OUTPUT);
  pinMode(bluLed, OUTPUT);
}
