#include <phys253.h>
#include <LiquidCrystal.h>
#include <avr/EEPROM.h>

const byte LEFT_MOTOR = 0;
const byte RIGHT_MOTOR = 1;
const byte SCISSOR_MOTOR = 2;
const byte CLAW_MOTOR = 3;
const byte TEN_KHZ_PIN = 7; // Analog 0
const byte ONE_KHZ_PIN = 6;
const byte CLAW_POT = 5;
const byte LEFT_LF_QRD = 1; // line following qrds
const byte RIGHT_LF_QRD = 0;
const byte LEFT_EDGE_QRD = 2; // edge avoiding qrds
const byte RIGHT_EDGE_QRD = 3;
const byte EWOK_DETECTOR = 4;
const byte SCISSOR_BUMP = 4; // Digital 5
const byte FRONT_BUMP = 5;
const byte LEFT_US_TRIG = 9;
const byte RIGHT_US_TRIG = 8;
const byte LEFT_US_ECHO = 7;
const byte RIGHT_US_ECHO = 6;
uint16_t LEFT_EDGE_THRESH, RIGHT_EDGE_THRESH;
uint16_t CLAW_LEFT = 285;
uint16_t CLAW_UP_LEFT = 615;
uint16_t CLAW_UP_RIGHT = 675;
uint16_t CLAW_RIGHT = 970;
const byte CLAW_SERVO_OPEN = 115;
const byte CLAW_SERVO_CLOSED = 165;
const byte BRIDGE1_SERVO_OPEN = 2;
const byte BRIDGE1_SERVO_CLOSED = 90;
const byte BRIDGE2_SERVO_OPEN = 90;
const byte BRIDGE2_SERVO_CLOSED = 2;
uint8_t MOTOR_BASE_LEFT;
uint8_t MOTOR_BASE_RIGHT;
uint8_t CLAW_SPEED = 190;
volatile int32_t pos;
uint16_t tenkhzread, onekhzread;
int16_t leftSpeed, rightSpeed;
double error, prevErr, Out, errSum;
volatile boolean offtape;
volatile uint32_t prevTime, nextencode, timer, timerbegin, offtapebegin, offtapetimer;
uint32_t testtime0, testtime1, testtime2, testtime3, testtime4;
enum RobotState
{
  State_Ewok1,
  State_EdgeAlign1,
  State_EdgeAlign2,
  State_Bridge1Align,
  State_Bridge1Place,
  State_Ewok2,
  State_RightEdgeFollow,
  State_Ewok3,
  State_Zipline1,
  State_Bridge2Align,
  State_Bridge2Place,
  State_Ewok4,
  State_Chewbacca,
  State_Zipline2,
  State_Testing0,
  State_Testing1,
  State_Testing2,
  State_Testing3,
  State_Testing4,
  State_Testing5
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
MenuItem LeftMotorOffset = MenuItem("LeftOffset");
MenuItem RightMotorOffset = MenuItem("RightOffset");
MenuItem menuItems[] =
{
  MotorMax, MotorBase, ProportionalGain, DerivativeGain, IntegralGain,
  ThreshTape, ThreshLEdge, ThreshREdge,
  RunScissor, RunLeft, RunRight, Timer, Servo0, Servo1, Servo2, Claw,
  StartState, TestTime0, TestTime1, TestTime2, TestTime3, TestTime4,
  LeftMotorOffset, RightMotorOffset
};

void setup()
{
#include <phys253setup.txt>
  RCServo1.write(BRIDGE1_SERVO_CLOSED);
  RCServo2.write(BRIDGE2_SERVO_CLOSED);
  pinMode(LEFT_LF_QRD, INPUT);
  pinMode(RIGHT_LF_QRD, INPUT);
  pinMode(LEFT_EDGE_QRD, INPUT);
  pinMode(RIGHT_EDGE_QRD, INPUT);
  pinMode(TEN_KHZ_PIN, INPUT);
  pinMode(ONE_KHZ_PIN, INPUT);
  pinMode(SCISSOR_BUMP, INPUT);
  pinMode(LEFT_US_ECHO, INPUT);
  pinMode(LEFT_US_TRIG, OUTPUT);
  pinMode(RIGHT_US_ECHO, INPUT);
  pinMode(RIGHT_US_TRIG, OUTPUT);
  statecontrol = (RobotState) (StartState.Value);
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
  MOTOR_BASE_LEFT = MotorBase.Value + LeftMotorOffset.Value;
  MOTOR_BASE_RIGHT = MotorBase.Value + RightMotorOffset.Value;
  ClawRotate(1);
  RCServo0.write(CLAW_SERVO_OPEN);
  delay(1000);

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
    case State_Ewok1: {
        TapeFollow();
        Ewok1Detect();
      } break;
    case State_EdgeAlign1: {
        boolean left = analogRead(LEFT_EDGE_QRD) > LEFT_EDGE_THRESH;
        boolean right = analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH;
        LCD.print(analogRead(LEFT_EDGE_QRD));
        LCD.print("/");
        LCD.print(analogRead(RIGHT_EDGE_QRD));
        if (left && right) {
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          delay(500);
          statecontrol = State_EdgeAlign2;
        } else if (left) {
          Pivot(0, 1);
        } else if (right) {
          Pivot(1, 1);
        } else {
          DriveStraight(1);
        }
      } break;
    case State_EdgeAlign2: {
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
          ReverseStraight(400);
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          delay(500);
          LCD.print("Edge Aligned");
          while (analogRead(LEFT_EDGE_QRD) < LEFT_EDGE_THRESH) {
            ZeroTurn(0, 1);
          }
          statecontrol = State_Bridge1Align;
        }
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
          delay(500);
          statecontrol = State_Bridge1Place;
        } else if (left) {
          Pivot(0, 1);
        } else if (right) {
          Pivot(1, 1);
        } else {
          DriveStraight(1);
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
          ZeroTurn(1, 1);
        } else if (left) {
          ZeroTurn(0, 1);
        } else {
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          delay(500);
          /*
            Pivot(1,100);
            motor.stop(LEFT_MOTOR);
            motor.stop(RIGHT_MOTOR);
            delay(500);
          */
          ReverseStraight(250);
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          delay(500);
          RCServo1.write(BRIDGE1_SERVO_OPEN);
          delay(2000);
          DriveStraight(500);
          FindTape(1);
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          ClawRotate(1);
          timerbegin = millis();
          while (millis() - timerbegin < 500) {
            TapeFollow();
          }
          timerbegin = millis();
          while (millis() - timerbegin < 1000) {
            TapeFollowEdge();
          }
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          delay(500);
          ZeroTurn(1,50
          );
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          delay(500);
          statecontrol = State_Ewok2;
        }
      } break;
    case State_Ewok2: {
      DriveStraight(1);
      Ewok2Detect();
      } break;
    case State_RightEdgeFollow: {
        EdgeFollow();
        if (!FRONT_BUMP) {
          motor.speed(LEFT_MOTOR, -MOTOR_BASE_LEFT / 2);
          motor.speed(RIGHT_MOTOR, -MOTOR_BASE_RIGHT);
          delay(300);
        }
      } break;
    case State_Ewok3: {
        TapeFollow();
        Ewok3Detect();
      } break;
    case State_Zipline1: {
        Zipline1Detect();
      } break;
    case State_Bridge2Align: {
        boolean left = analogRead(LEFT_EDGE_QRD) > LEFT_EDGE_THRESH;
        boolean right = analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH;
        LCD.print(analogRead(LEFT_EDGE_QRD));
        LCD.print("/");
        LCD.print(analogRead(RIGHT_EDGE_QRD));
        if (left && right) {
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          statecontrol = State_Bridge2Place;
        } else if (left) {
          Pivot(0, 1);
        } else if (right) {
          Pivot(1, 1);
        } else {
          DriveStraight(1);
        }
      } break;
    case State_Bridge2Place: {
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
          RCServo1.write(BRIDGE1_SERVO_OPEN);
          delay(500);
          statecontrol = State_Ewok4;
        }
      } break;
    case State_Ewok4: {
        Ewok4Detect();
      } break;
    case State_Chewbacca: {
        EdgeAvoid();
        //EwokDetect();
      } break;
    case State_Zipline2: {
        Zipline2Detect();
      } break;
    case State_Testing0: {
        LCD.print(analogRead(CLAW_POT));
        LCD.setCursor(0, 1);
        LCD.print(analogRead(EWOK_DETECTOR));
        delay(500);
      } break;
    case State_Testing1: {
        digitalWrite(RIGHT_US_TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(RIGHT_US_TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(RIGHT_US_TRIG, LOW);
        uint16_t rightdist = pulseIn(RIGHT_US_ECHO, HIGH) * .034 / 2;

        digitalWrite(LEFT_US_TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(LEFT_US_TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(LEFT_US_TRIG, LOW);
        uint16_t leftdist = pulseIn(LEFT_US_ECHO, HIGH) * .034 / 2;

        LCD.print(leftdist);
        LCD.setCursor(0, 1);
        LCD.print(rightdist);
        delay(500);
      } break;
    case State_Testing2: {
        LCD.print(analogRead(LEFT_LF_QRD));
        LCD.print("/");
        LCD.print(analogRead(RIGHT_LF_QRD));
        LCD.setCursor(0, 1);
        LCD.print(analogRead(LEFT_EDGE_QRD));
        LCD.print("/");
        LCD.print(analogRead(RIGHT_EDGE_QRD));
      } break;
    case State_Testing3: {
        LCD.print(analogRead(ONE_KHZ_PIN));
        LCD.setCursor(0, 1);
        LCD.print(analogRead(TEN_KHZ_PIN));
        delay(500);
      } break;
    case State_Testing4: {
        LCD.print("DANCE!");
        Dance();
      } break;
    case State_Testing5: {
        DriveStraight(2000);
        delay(2000);
        ReverseStraight(2000);
        delay(2000);
      } break;
  }
}
