#include <phys253.h>
#include <LiquidCrystal.h>
#include <avr/EEPROM.h>

#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1
#define LEFT_LF_QRD 4
#define RIGHT_LF_QRD 5

uint8_t leftSpeed,rightSpeed;
double error,prevErr,Out;
double errSum;
uint32_t prevTime;
boolean left, right;
boolean stopped = 0;

class MenuItem
{
public:
  String    Name;
  uint8_t  Value;
  uint16_t* EEPROMAddress;
  static uint16_t MenuItemCount;
  MenuItem(String name)
  {
    MenuItemCount++;
    EEPROMAddress = (uint16_t*)(2 * MenuItemCount);
    Name      = name;
    Value         = eeprom_read_word(EEPROMAddress);
  }
  void Save()
  {
    eeprom_write_word(EEPROMAddress, Value);
  }
};
 
uint16_t MenuItem::MenuItemCount = 0;
/* Add the menu items here */
MenuItem MotorMax         = MenuItem("MotorMax");
MenuItem MotorBase        = MenuItem("MotorBase");
MenuItem ProportionalGain = MenuItem("P-gain");
MenuItem DerivativeGain   = MenuItem("D-gain");
MenuItem IntegralGain     = MenuItem("I-gain");
MenuItem menuItems[]      = {MotorMax, MotorBase, ProportionalGain, DerivativeGain, IntegralGain};

void setup()
{
#include <phys253setup.txt>
  Serial.begin(9600) ;
  pinMode(LEFT_MOTOR,OUTPUT);
  pinMode(RIGHT_MOTOR,OUTPUT);
  pinMode(LEFT_LF_QRD,INPUT);
  pinMode(RIGHT_LF_QRD,INPUT);
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
  
  LCD.print("LS:");
  LCD.print(analogRead(LEFT_LF_QRD));
  LCD.setCursor(8,0);
  LCD.print("RS:");
  LCD.print(analogRead(RIGHT_LF_QRD));
 
  if (analogRead(RIGHT_LF_QRD) >100){
    right = 1;
  } else{
    right = 0;
  }
  if (analogRead(LEFT_LF_QRD)>100){
    left = 1;
  } else {
    left = 0;
  }
  if (left && right){
    error = 0;
  } else if (left && !right){
    error = -1;
  } else if (!left && right){
    error = 1;
  } else {
    if (prevErr>0){
      error = 5;
    } else {
      error = -5;
    }
  }
  update();
  leftSpeed = MotorBase.Value + Out;
  rightSpeed = MotorBase.Value - Out;
  if (rightSpeed>MotorMax.Value){
    rightSpeed = MotorMax.Value;
  } else if (rightSpeed<0){
    rightSpeed = 0;
  }
  if (leftSpeed>MotorMax.Value){
    leftSpeed = MotorMax.Value;
  } else if (leftSpeed<0){
    leftSpeed = 0;
  }
  LCD.setCursor(0,1);
  motor.speed(LEFT_MOTOR,leftSpeed);
  motor.speed(RIGHT_MOTOR,rightSpeed);
  LCD.print("LM:");
  LCD.print(leftSpeed);
  LCD.setCursor(8,1);
  LCD.print("RM:");
  LCD.print(rightSpeed);
}

void update(){
  unsigned long now = millis();
  double dtime = (double) (now - prevTime);

  errSum += (error * dtime);
  double dErr = (error - prevErr)/dtime;

  Out = ProportionalGain.Value * error + IntegralGain.Value * errSum + DerivativeGain.Value * dErr;

  prevErr = error;
  prevTime = now;
}

void Menu()
{
  LCD.clear(); LCD.home();
  LCD.print("Entering menu");
  delay(500);

  motor.speed(LEFT_MOTOR,0);
  motor.speed(RIGHT_MOTOR,0);

  while (true)
  {
    /* Show MenuItem value and knob value */
    int menuIndex = knob(6) * (MenuItem::MenuItemCount) / 1024;
    LCD.clear(); LCD.home();
    LCD.print(menuItems[menuIndex].Name); LCD.print(" "); LCD.print(menuItems[menuIndex].Value);
    LCD.setCursor(0, 1);
    LCD.print("Set to "); LCD.print(knob(7)/4); LCD.print("?");
    delay(100);

    /* Press start button to save the new value */
    if (startbutton())
    {
      delay(100);
      if (startbutton())
      {
        menuItems[menuIndex].Value = knob(7)/4;
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
        LCD.clear(); LCD.home();
        LCD.print("Leaving menu");
        delay(500);
        return;
      }
    }
  }
}
