#include <phys253.h>
#include <LiquidCrystal.h>
#include <avr/EEPROM.h>
#include <Wire.h>

#define QRD_THRESH 150;

const byte LEFT_MOTOR = 0;
const byte RIGHT_MOTOR = 1;
const byte SCISSOR_MOTOR = 2;
const byte LEFT_LF_QRD = 4; // line following qrds
const byte RIGHT_LF_QRD = 5;
const byte LEFT_EDGE_QRD = 3; // edge avoiding qrds
const byte RIGHT_EDGE_QRD = 6;
const byte TEN_KHZ_PIN = 0; // analog 7
const byte ONE_KHZ_PIN = 1;
const byte BLUE_ADDR1 = 2; // i2c address of blue pill
const byte ENCODER_A = 2;
const byte ENCODER_B = 3;
const byte SCISSOR_BUMP = 15;
volatile int32_t pos;
uint16_t tenkhzread, onekhzread;
int16_t leftSpeed, rightSpeed;
double error, prevErr, Out, errSum;
volatile uint32_t prevTime, nextencode;
enum RobotState
{
  State_TapeFollow,
  State_IRDetect,
  State_Bridge1,
  State_Bridge2,
  State_EdgeAvoid,
  State_Zipline,
  State_EwokRetrieval,
  State_Testing
};

RobotState statecontrol;
class MenuItem
{
  public:
    String Name;
    uint8_t Value;
    uint16_t * EEPROMAddress;
    static uint16_t MenuItemCount;
    MenuItem(String name)
    {
      MenuItemCount++;
      EEPROMAddress = (uint16_t *) (2 * MenuItemCount);
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
MenuItem RewindScissor = MenuItem("Rewind");
MenuItem LiftScissor = MenuItem("Lift");
MenuItem menuItems[] =
{
  MotorMax, MotorBase, ProportionalGain, DerivativeGain, IntegralGain, RewindScissor, LiftScissor
};

void setup()
{
#include <phys253setup.txt>
  RCServo0.write(0);
  RCServo1.write(90);
  RCServo2.write(0);
  pinMode(LEFT_LF_QRD, INPUT);
  pinMode(RIGHT_LF_QRD, INPUT);
  pinMode(LEFT_EDGE_QRD, INPUT);
  pinMode(RIGHT_EDGE_QRD, INPUT);
  pinMode(TEN_KHZ_PIN, INPUT);
  pinMode(ONE_KHZ_PIN, INPUT);
  pinMode(ENCODER_A, INPUT);
  pinMode(ENCODER_B, INPUT);
  pinMode(SCISSOR_BUMP, INPUT);
  statecontrol = State_Testing;
  Wire.begin();
  attachInterrupt(INT2, encoder, FALLING);
  pos = 0;
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
    case State_TapeFollow: {
        TapeFollow();
        //EwokDetect();
      } break;
    case State_IRDetect: {
        TapeFollow();
        IRBeacon();
        EwokDetect();
      } break;
    case State_Bridge1: {
        // Align QRDs so all four read black/off the edge
        boolean left = analogRead(LEFT_EDGE_QRD) > QRD_THRESH;
        boolean right = analogRead(RIGHT_EDGE_QRD) > QRD_THRESH;
        LCD.print(analogRead(LEFT_EDGE_QRD));
        LCD.print("/");
        LCD.print(analogRead(LEFT_LF_QRD));
        LCD.setCursor(0, 1);
        LCD.print(analogRead(RIGHT_EDGE_QRD));
        LCD.print("/");
        LCD.print(analogRead(RIGHT_LF_QRD));
        if (left && right) {
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          RCServo1.write(0); // Lower bridge
          DriveStraight(5000);
        } else if (left) {
          Pivot(0, 200);
        } else if (right) {
          Pivot(1, 200);
        } else {
          TapeFollow();
        }
      } break;
    case State_Bridge2: {
        // Align QRDs so all four read black/off the edge
        // Detect zipline above?
        RCServo2.write(90); // Lower bridge
        // Back up to place bridge
      } break;
    case State_EdgeAvoid: {
        EdgeAvoid();
        EwokDetect();
      } break;
    case State_EwokRetrieval: {
        EwokRetrieve();
        // Once Ewok has been retrieved, return to original state.
      } break;
    case State_Zipline: {
        ZipAlign();
        // If zipline has been aligned, ZiplinePlace()
      } break;
    case State_Testing: {
      } break;
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
    if (menuItems[menuIndex].Name.equals("Rewind")) {
      if (startbutton()) {
        delay(100);
        while (startbutton()) {
          motor.speed(SCISSOR_MOTOR, -255);
        }
        motor.stop(SCISSOR_MOTOR);
      }
    }  else if (menuItems[menuIndex].Name.equals("Lift")) {
      if (startbutton()) {
        delay(100);
        while (startbutton()) {
          motor.speed(SCISSOR_MOTOR, 255);
        }
        motor.stop(SCISSOR_MOTOR);
      }
    }
    else {
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
}

void encoder() {
  if (millis() > nextencode) {
    pos++;
    nextencode = millis() + 10;
  }
}




