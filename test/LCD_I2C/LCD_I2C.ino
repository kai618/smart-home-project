#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

  Serial.begin(115200);
  Wire.begin(2, 3);

  lcd.begin();
  lcd.home();
  lcd.print("Hello, NodeMCU");

}

void loop() {
  
}
