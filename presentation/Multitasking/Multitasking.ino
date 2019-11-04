// led 1 will switch every 2s
// led 2 will switch every 3s

#include <Chrono.h>

void setup() {
  Serial.begin(115200);
  // set led pin
}

void loop() {
  switchLed1();
  switchLed2();
}

Chrono timer1;

void switchLed1() {
  if (timer1.hasPassed(2000)) {  
    timer1.restart();  
    Serial.println("Switch led 1");    
  }
}

Chrono timer2;

void switchLed2() {
  if (timer2.hasPassed(3000)) {   
    timer2.restart(); 
    Serial.println("Switch led 222222222222222222222222");    
  }
}

//unsigned long startTime1 = 0;
//
//void switchLed1() {
//  if (millis() - startTime1 >= 2000) {
//    startTime1 = millis();
//    Serial.println("Switch led 1");
//  }
//}
//
//unsigned long startTime2 = 0;
//
//void switchLed2() {
//  if (millis() - startTime2 >= 3000) {
//    startTime2 = millis();
//    Serial.println("Switch led 22222222222222222222222");
//  }
//}
