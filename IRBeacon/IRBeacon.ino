#include <phys253.h>
#include <LiquidCrystal.h>

#define TEN_KHZ_PIN 5 //analog 2
#define ONE_KHZ_PIN 6 //analog 1

uint16_t tenkhzread,onekhzread;

void setup()
{
#include <phys253setup.txt>
  Serial.begin(9600) ;
  pinMode(TEN_KHZ_PIN,INPUT);
  pinMode(ONE_KHZ_PIN,INPUT);
}

void loop()
{
  LCD.clear();
  tenkhzread = analogRead(TEN_KHZ_PIN);
  onekhzread = analogRead(ONE_KHZ_PIN);
  LCD.print("1KHZ:");
  LCD.print(onekhzread);
  LCD.setCursor(0,1);
  LCD.print("10KHZ:");
  LCD.print(tenkhzread);
  delay(100);
}
