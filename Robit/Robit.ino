#include <phys253.h>
#include <LiquidCrystal.h>
#include <avr/EEPROM.h>
#include <Wire.h>

const byte LEFT_MOTOR = 0;
const byte RIGHT_MOTOR = 1;
const byte SCISSOR_MOTOR = 2;
const byte CLAW_MOTOR = 3;
const byte LEFT_LF_QRD = 4; // line following qrds
const byte RIGHT_LF_QRD = 5;
const byte LEFT_EDGE_QRD = 3; // edge avoiding qrds
const byte RIGHT_EDGE_QRD = 6;
const byte CLAW_POT = 8;
const byte EWOK_DETECTOR = 2;
const byte TEN_KHZ_PIN = 0; // analog 7
const byte ONE_KHZ_PIN = 1;
const byte BLUE_ADDR1 = 2; // i2c address of blue pill
const byte ENCODER_A = 2;
const byte ENCODER_B = 3;
const byte SCISSOR_BUMP = 15;
uint16_t LEFT_EDGE_THRESH, RIGHT_EDGE_THRESH;
uint16_t CLAW_LEFT, CLAW_UP, CLAW_RIGHT;
volatile int32_t pos;
uint16_t tenkhzread, onekhzread;
int16_t leftSpeed, rightSpeed;
double error, prevErr, Out, errSum;
volatile boolean offtape;
volatile uint32_t prevTime, nextencode, timer, timerbegin, offtapebegin, offtapetimer;
uint32_t testtime0, testtime1, testtime2, testtime3, testtime4;
enum RobotState
{
  State_Begin,
  State_IRDetect,
  State_Bridge1Align,
  State_Bridge1Place,
  State_EdgeAvoid,
  State_Zipline,
  State_EwokRetrieval,
  State_Testing0,
  State_Testing1,
  State_Testing2,
  State_Testing3,
  State_Testing4
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
MenuItem ThreshTape = MenuItem("TapeThresh");
MenuItem ThreshLEdge = MenuItem("LEdgeThresh");
MenuItem ThreshREdge = MenuItem("REdgeThresh");
MenuItem RunScissor = MenuItem("RunScissor");
MenuItem Claw = MenuItem("RunClaw");
MenuItem RunLeft = MenuItem("RunLeft");
MenuItem RunRight = MenuItem("RunRight");
MenuItem Timer = MenuItem("RunTimer");
MenuItem Servo0 = MenuItem("RunServo0");
MenuItem Servo1 = MenuItem("RunServo1");
MenuItem Servo2 = MenuItem("RunServo2");
MenuItem StartState = MenuItem("StartState");
MenuItem TestTime0 = MenuItem("TestTime0");
MenuItem TestTime1 = MenuItem("TestTime1");
MenuItem TestTime2 = MenuItem("TestTime2");
MenuItem TestTime3 = MenuItem("TestTime3");
MenuItem TestTime4 = MenuItem("TestTime4");
MenuItem menuItems[] =
{
  MotorMax, MotorBase, ProportionalGain, DerivativeGain, IntegralGain,
  ThreshTape, ThreshLEdge, ThreshREdge,
  RunScissor, RunLeft, RunRight, Timer, Servo0, Servo1, Servo2, Claw,
  StartState, TestTime0, TestTime1, TestTime2, TestTime3, TestTime4
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
  statecontrol = (RobotState) (StartState.Value);
  Wire.begin();
  attachInterrupt(INT2, encoder, FALLING);
  pos = 0;
  pinMode(35, OUTPUT);
  pinMode(14, INPUT);
  timer = 0;
  timerbegin = millis();
  LEFT_EDGE_THRESH = (uint16_t) ThreshLEdge.Value * 2;
  RIGHT_EDGE_THRESH = (uint16_t) ThreshREdge.Value * 2;
  testtime0 = (uint32_t) TestTime0.Value * 100;
  testtime1 = (uint32_t) TestTime1.Value * 100;
  testtime2 = (uint32_t) TestTime2.Value * 100;
  testtime3 = (uint32_t) TestTime3.Value * 100;
  testtime4 = (uint32_t) TestTime4.Value * 100;
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
    case State_Begin: {
        TapeFollow();
        EwokDetect();
      } break;
    case State_IRDetect: {
        //TapeFollow();
        IRBeacon();
        //EwokDetect();
      } break;
    case State_Bridge1Align: {
        boolean left = analogRead(LEFT_EDGE_QRD) > LEFT_EDGE_THRESH;
        boolean right = analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH;
        LCD.print(analogRead(LEFT_EDGE_QRD));
        LCD.print("/");
        LCD.print(analogRead(RIGHT_EDGE_QRD));
        if (left && right) {
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          statecontrol = State_Bridge1Place;
        } else if (left) {
          Pivot(0, 1);
        } else if (right) {
          Pivot(1, 1);
        } else {
          TapeFollow();
        }
      } break;
    case State_Bridge1Place: {
        boolean left = analogRead(LEFT_EDGE_QRD) > LEFT_EDGE_THRESH;
        boolean right = analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH;
        LCD.print(analogRead(LEFT_EDGE_QRD));
        LCD.print("/");
        LCD.print(analogRead(RIGHT_EDGE_QRD));
        if (left && right) {
          ReverseStraight(1);
        } else if (right) {
          PivotBack(0, 1);
        } else if (left) {
          PivotBack(1, 1);
        } else {
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          LCD.print("Edge Aligned");
          ReverseStraight(200);
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          RCServo1.write(0);
          delay(500);
          DriveStraight(2000);
        }
      } break;
    case State_EdgeAvoid: {
        EdgeAvoid();
        //EwokDetect();
      } break;
    case State_EwokRetrieval: {
        EwokRetrieve();
        // Once Ewok has been retrieved, return to original state.
      } break;
    case State_Zipline: {
        ScissorLift(1);
        delay(1000);
        motor.speed(LEFT_MOTOR, -MotorBase.Value);
        motor.speed(RIGHT_MOTOR, -MotorBase.Value);
        delay(1000);
        motor.stop(LEFT_MOTOR);
        motor.stop(RIGHT_MOTOR);
        delay(1000);
        ScissorLift(0);
        // If zipline has been aligned, ZiplinePlace()
      } break;
    case State_Testing0: {
        IRBeacon();
      } break;
    case State_Testing1: {
      } break;
    case State_Testing2: {
        DriveStraight(1);
      } break;
    case State_Testing3: {
        FindTape(1);
        motor.stop(LEFT_MOTOR);
        motor.stop(RIGHT_MOTOR);
        delay(2000);
      } break;
    case State_Testing4: {
        LCD.print("DANCE!");
        Dance();
      } break;
  }
}

void encoder() {
  if (millis() > nextencode) {
    pos++;
    nextencode = millis() + 10;
  }
}










