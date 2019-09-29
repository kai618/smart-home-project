// We should scan for the address of I2C
// NodeMCU 0x3F
// Arduino 0x27
// control the LCD contrast by adjust the on-board potentionmeter

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

float temp = 12.35;
float humid = 56.45;
bool hasLabel = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(D7, D6);
  lcd.begin();
  lcd.home();
}

void loop() { 
  printTempHumid();
}

void printTempHumid() {
  if (!hasLabel) {
    lcd.setCursor(1,0);
    lcd.print("Temp :       C");
    lcd.setCursor(13,0);
    lcd.print(char(223));
    lcd.setCursor(1,1);
    lcd.print("Humid:      %");
    hasLabel = true;
  }
  lcd.setCursor(8,0);
  lcd.print(temp);
  lcd.setCursor(8,1);
  lcd.print(humid);
}
