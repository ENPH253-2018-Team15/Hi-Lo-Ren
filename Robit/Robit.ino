#include <phys253.h>
#include <LiquidCrystal.h>
#include <avr/EEPROM.h>

#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1
#define LEFT_LF_QRD 4
#define RIGHT_LF_QRD 5
#define TEN_KHZ_PIN 6 //analog 1
#define ONE_KHZ_PIN 7 //analog 0

uint16_t tenkhzread, onekhzread;
uint8_t leftSpeed, rightSpeed;
double error, prevErr, Out;
double errSum;
uint32_t prevTime;
boolean left, right;

enum RobotState{
  State_TapeFollow,
  State_IRDetect,
  State_Testing
};
RobotState statecontrol;

class MenuItem
{
public:
  String Name;
  uint8_t Value;
  uint16_t *EEPROMAddress;
  static uint16_t MenuItemCount;
  MenuItem(String name)
  {
    MenuItemCount++;
    EEPROMAddress = (uint16_t *)(2 * MenuItemCount);
    Name = name;
    Value = eeprom_read_word(EEPROMAddress);
  }
  void Save()
  {
    eeprom_write_word(EEPROMAddress, Value);
  }
};

uint16_t MenuItem::MenuItemCount = 0;
/* Add the menu items here */
MenuItem MotorMax = MenuItem("MotorMax");
MenuItem MotorBase = MenuItem("MotorBase");
MenuItem ProportionalGain = MenuItem("P-gain");
MenuItem DerivativeGain = MenuItem("D-gain");
MenuItem IntegralGain = MenuItem("I-gain");
MenuItem menuItems[] = {MotorMax, MotorBase, ProportionalGain, DerivativeGain, IntegralGain};

void setup()
{
#include <phys253setup.txt>
  Serial.begin(9600);
  pinMode(LEFT_MOTOR, OUTPUT);
  pinMode(RIGHT_MOTOR, OUTPUT);
  pinMode(LEFT_LF_QRD, INPUT);
  pinMode(RIGHT_LF_QRD, INPUT);
  pinMode(TEN_KHZ_PIN, INPUT);
  pinMode(ONE_KHZ_PIN, INPUT);
  statecontrol = State_TapeFollow;
}

void loop()
{
  LCD.clear();

  if (startbutton())
  {
    delay(100);
    if (startbutton())
    {
      Menu();
    }
  }

  switch (statecontrol)
  {
  case State_TapeFollow:
    TapeFollow();
    break;
  case State_IRDetect:
    TapeFollow();
    IRBeacon();
    break;
  case State_Testing:
    TapeFollow();
    break;
  }
}

void Menu()
{
  LCD.clear();
  LCD.home();
  LCD.print("Entering menu");
  delay(500);

  motor.speed(LEFT_MOTOR, 0);
  motor.speed(RIGHT_MOTOR, 0);

  while (true)
  {
    /* Show MenuItem value and knob value */
    int menuIndex = knob(6) * (MenuItem::MenuItemCount) / 1024;
    LCD.clear();
    LCD.home();
    LCD.print(menuItems[menuIndex].Name);
    LCD.print(" ");
    LCD.print(menuItems[menuIndex].Value);
    LCD.setCursor(0, 1);
    LCD.print("Set to ");
    LCD.print(knob(7) / 4);
    LCD.print("?");
    delay(100);

    /* Press start button to save the new value */
    if (startbutton())
    {
      delay(100);
      if (startbutton())
      {
        menuItems[menuIndex].Value = knob(7) / 4;
        menuItems[menuIndex].Save();
        delay(250);
      }
    }

    /* Press stop button to exit menu */
    if (stopbutton())
    {
      delay(100);
      if (stopbutton())
      {
        LCD.clear();
        LCD.home();
        LCD.print("Leaving menu");
        delay(500);
        return;
      }
    }
  }
}
