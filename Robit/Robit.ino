
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
uint16_t ONE_KHZ_THRESH = 50;
uint16_t TEN_KHZ_THRESH = 150;
uint16_t CLAW_LEFT = 290;
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
uint32_t FINDTAPETIME = 2000;
volatile int32_t pos;
uint16_t tenkhzread, onekhzread;
int16_t leftSpeed, rightSpeed;
double error, prevErr, Out, errSum;
volatile boolean offtape;
volatile uint32_t prevTime, nextencode, timer, timerbegin, offtapebegin, offtapetimer;
uint32_t testtime0, testtime1, testtime2, testtime3, testtime4, testtime5;
enum RobotState
{
  State_Start,
  State_Begin,
  State_Ewok1,
  State_EdgeAlign1,
  State_EdgeAlign2,
  State_Bridge1Align,
  State_Bridge1Place,
  State_Ewok2,
  State_Archway,
  State_Ewok3,
  State_Edge2Align1,
  State_Edge2Align2,
  State_Bridge2Align,
  State_Bridge2Place,
  State_Ewok4,
  State_SuspensionBridge,
  State_Chewbacca,
  State_Zipline2,
  State_Testing0,
  State_Testing1,
  State_Testing2,
  State_Testing3,
  State_Testing4,
  State_Testing5,
  State_Testing6
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
MenuItem TestTime5 = MenuItem("TestTime5");
MenuItem LeftMotorOffset = MenuItem("LeftOffset");
MenuItem RightMotorOffset = MenuItem("RightOffset");
MenuItem menuItems[] =
{
  MotorMax, MotorBase, ProportionalGain, DerivativeGain, IntegralGain,
  ThreshTape, ThreshLEdge, ThreshREdge,
  RunScissor, RunLeft, RunRight, Timer, Servo0, Servo1, Servo2, Claw,
  StartState, TestTime0, TestTime1, TestTime2, TestTime3, TestTime4, TestTime5,
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
  testtime5 = (uint32_t) TestTime5.Value * 100;
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
    case State_Start: {
        if (stopbutton()) {
          delay(100);
          if (stopbutton()) {
            statecontrol = State_Begin;
            delay(500);
          }
        }
      } break;
    case State_Begin: {
        timerbegin = millis();
        while (millis() - timerbegin < 6750) {
          TapeFollow();
        }
        MOTOR_BASE_LEFT = MotorBase.Value + LeftMotorOffset.Value - 25;
        MOTOR_BASE_RIGHT = MotorBase.Value + RightMotorOffset.Value - 25;
        //        statecontrol = State_Testing1;
        statecontrol = State_Ewok1;
      } break;
    case State_Ewok1: {
        TapeFollow();
        Ewok1Detect();
      } break;
    case State_EdgeAlign1: {
        //        ReverseStraight(400);
        motor.stop(LEFT_MOTOR);
        motor.stop(RIGHT_MOTOR);
        delay(100);
        //Follow Right edge until left hits edge. Need this to align better for bridge and arch
        while (analogRead(LEFT_EDGE_QRD) < LEFT_EDGE_THRESH) {

          while (analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH) {
            Pivot(0, 10);
          }
          DriveStraight(30);
          while (analogRead(RIGHT_EDGE_QRD) < RIGHT_EDGE_THRESH) {
            Pivot(1, 10);
          }
          DriveStraight(30);
        }
        DriveStraight(100);
        motor.stop(LEFT_MOTOR);
        motor.stop(RIGHT_MOTOR);

        statecontrol = State_EdgeAlign2;

        // boolean left = analogRead(LEFT_EDGE_QRD) > LEFT_EDGE_THRESH;
        // boolean right = analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH;
        // LCD.print(analogRead(LEFT_EDGE_QRD));
        // LCD.print("/");
        // LCD.print(analogRead(RIGHT_EDGE_QRD));
        // if (left && right) {
        //   motor.stop(LEFT_MOTOR);
        //   motor.stop(RIGHT_MOTOR);
        //   delay(500);
        //   statecontrol = State_EdgeAlign2;
        // } else if (left) {
        //   Pivot(0, 1);
        // } else if (right) {
        //   Pivot(1, 1);
        // } else {
        //   DriveStraight(1);
        // }
      } break;
    case State_EdgeAlign2: {
        delay(50);
        ZeroTurn(0, 600);
        motor.stop(LEFT_MOTOR);
        motor.stop(RIGHT_MOTOR);
        timerbegin = millis();
        while ((analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH) && (millis() - timerbegin < 1000)) {
          LCD.clear();
          LCD.print("PIVOTING");
          Pivot(0, 10);
        }
        motor.stop(LEFT_MOTOR);
        motor.stop(RIGHT_MOTOR);

        statecontrol = State_Bridge1Align;
        //      boolean left = analogRead(LEFT_EDGE_QRD) > LEFT_EDGE_THRESH;
        //      boolean right = analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH;
        //      LCD.print(analogRead(LEFT_EDGE_QRD));
        //      LCD.print("/");
        //      LCD.print(analogRead(RIGHT_EDGE_QRD));
        //      if (left && right) {
        //        ReverseStraight(1);
        //      } else if (right) {
        //        PivotBack(0, 1);
        //      } else if (left) {
        //        PivotBack(1, 1);
        //      } else {
        //        //ReverseStraight(100);
        //        motor.stop(LEFT_MOTOR);
        //        motor.stop(RIGHT_MOTOR);
        //        delay(500);
        //        LCD.print("Edge Aligned");
        //        //ZeroTurn(0, 1000);
        //        ZeroTurn(0, 900);
        //        /*
        //          while (analogRead(LEFT_EDGE_QRD) < LEFT_EDGE_THRESH) {
        //          ZeroTurn(0, 1);
        //          }
        //        */
        //        statecontrol = State_Bridge1Align;
        //      }
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
          //ReverseStraight(300);
          ReverseStraight(275);
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          delay(500);
          RCServo1.write(BRIDGE1_SERVO_OPEN);
          delay(1500);
          DriveStraight(500);
          FindTape(1, 10000);
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          //ClawRotate(1);
          timerbegin = millis();
          while (millis() - timerbegin < 500) {
            TapeFollow();
          }
          DriveStraight(750);
          /*
            timerbegin = millis();
            while (millis() - timerbegin < 1000) {
            TapeFollowEdge();
            }
            motor.stop(LEFT_MOTOR);
            motor.stop(RIGHT_MOTOR);
            delay(500);
          */
          timerbegin = millis();
          while (millis() - timerbegin < 1500) {
            //2400
            EdgeTapeFollow();
          }
          while (!ArchDetect()) {
            EdgeTapeFollow();
          }
          timerbegin = millis();
          while (millis() - timerbegin < 275) {
            EdgeTapeFollow();
          }
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          RCServo1.write(BRIDGE1_SERVO_CLOSED);
          delay(1000);
          //Pivot(0, testtime1); //1500
          //ReverseStraight(testtime2); // 400
          //Pivot(0, 1500);
          //ReverseStraight(350);
          MOTOR_BASE_LEFT = MotorBase.Value + LeftMotorOffset.Value - 20;
          MOTOR_BASE_RIGHT = MotorBase.Value + RightMotorOffset.Value - 20;
          FindTape(0, 10000);
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          IRBeacon();
        }
      } break;
    case State_Ewok2: {
        if (FRONT_BUMP) {
          DriveStraight(1);
          //motor.speed(LEFT_MOTOR, MOTOR_BASE_LEFT * 8/6);
          //motor.speed(RIGHT_MOTOR, MOTOR_BASE_RIGHT * 4/6);
        }
        else {
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          ReverseStraight(100);
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          ZeroTurn(1, 50);
        }
        Ewok2Detect();
      } break;
    case State_Archway: {
        FINDTAPETIME = 2000;
        MOTOR_BASE_LEFT = MotorBase.Value + LeftMotorOffset.Value - 15;
        MOTOR_BASE_RIGHT = MotorBase.Value + RightMotorOffset.Value - 15;
        timerbegin = millis();
        while (millis() - timerbegin < 7000 || ((analogRead(LEFT_LF_QRD) < ThreshTape.Value && analogRead(RIGHT_LF_QRD) < ThreshTape.Value))) {
          TapeFollow();
        }
        motor.stop(LEFT_MOTOR);
        motor.stop(RIGHT_MOTOR);
        ClawRotate(-1);
        RCServo1.write(BRIDGE1_SERVO_OPEN);
        delay(500);
        FINDTAPETIME = 2000;
        MOTOR_BASE_LEFT = MotorBase.Value + LeftMotorOffset.Value;
        MOTOR_BASE_RIGHT = MotorBase.Value + RightMotorOffset.Value;
        FindTape(0, 150);
        statecontrol = State_Ewok3;
      } break;
    case State_Ewok3: {
        TapeFollow();
        Ewok3Detect();
      } break;
    case State_Edge2Align1: {
        boolean left = analogRead(LEFT_EDGE_QRD) > LEFT_EDGE_THRESH;
        boolean right = analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH;
        LCD.print(analogRead(LEFT_EDGE_QRD));
        LCD.print("/");
        LCD.print(analogRead(RIGHT_EDGE_QRD));
        if (left && right) {
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          delay(500);
          statecontrol = State_Edge2Align2;
        } else if (left) {
          Pivot(0, 1);
        } else if (right) {
          Pivot(1, 1);
        } else {
          DriveStraight(1);
        }
      } break;
    case State_Edge2Align2: {
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
          //ReverseStraight(100);
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          delay(500);
          LCD.print("Edge Aligned");

          ZeroTurn(1, 600);
          /*
            while (analogRead(LEFT_EDGE_QRD) < LEFT_EDGE_THRESH) {
            ZeroTurn(0, 1);
            }
          */
          statecontrol = State_Bridge2Align;
        }
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
          delay(1000);
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
          RCServo1.write(BRIDGE1_SERVO_OPEN);
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          delay(1000);
          LCD.print("Edge Aligned");
          ReverseStraight(100);
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          RCServo2.write(BRIDGE2_SERVO_OPEN);
          delay(1000);
          ReverseStraight(500);
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          RCServo1.write(BRIDGE1_SERVO_CLOSED);
          RCServo2.write(BRIDGE2_SERVO_CLOSED);
          delay(500);
          MOTOR_BASE_LEFT = MotorBase.Value + LeftMotorOffset.Value + 25;
          MOTOR_BASE_RIGHT = MotorBase.Value + RightMotorOffset.Value + 25;
          DriveStraight(750);
          FindTape(0, 500);
          while (analogRead(LEFT_LF_QRD) > ThreshTape.Value || analogRead(RIGHT_LF_QRD) > ThreshTape.Value) {
            TapeFollow();
          }
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          delay(500);
          //DriveStraight(1000);
          DriveStraight(1500);
          while (analogRead(LEFT_EDGE_QRD) < LEFT_EDGE_THRESH || analogRead(RIGHT_EDGE_QRD) < RIGHT_EDGE_QRD) {
            DriveStraight(1);
          }
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          delay(1000);
          ReverseStraight(500);
          Pivot(1, 700);
          MOTOR_BASE_LEFT = MotorBase.Value + LeftMotorOffset.Value;
          MOTOR_BASE_RIGHT = MotorBase.Value + RightMotorOffset.Value;
          timerbegin = millis();
          while (millis() - timerbegin < 3500) {
            EdgeAvoid();
          }
          /*
            while (analogRead(LEFT_EDGE_QRD) < LEFT_EDGE_THRESH && analogRead(RIGHT_EDGE_QRD) < RIGHT_EDGE_QRD) {
            DriveStraight(1);
            }
            MOTOR_BASE_LEFT = MotorBase.Value + LeftMotorOffset.Value;
            MOTOR_BASE_RIGHT = MotorBase.Value + RightMotorOffset.Value;
            DriveStraight(500);
            motor.stop(LEFT_MOTOR);
            motor.stop(RIGHT_MOTOR);
            ClawRotate(-1);
            delay(1000);
            statecontrol = State_Ewok4;
          */
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          ClawRotate(-1);
          statecontrol = State_SuspensionBridge;
        }
      } break;
    case State_Ewok4: {
        timerbegin = millis();
        while (millis() - timerbegin < 1000) {
          Pivot(1, 10);
          Ewok4Detect();
        }
        statecontrol = State_SuspensionBridge;
      } break;
    case State_SuspensionBridge: {
        EdgeAvoid();
        LCD.clear();
        LCD.home();
        LCD.print("EdgeAvoiding");
        //Keep pinging the US until Zipline2 is detected above
        digitalWrite(RIGHT_US_TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(RIGHT_US_TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(RIGHT_US_TRIG, LOW);
        uint16_t rightdist = pulseIn(RIGHT_US_ECHO, HIGH) * .034 / 2;
        if (rightdist < 30) {
          LCD.clear();
          LCD.home();
          LCD.print("ZIPLINE_FOUND");
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          delay(500);
          while (analogRead(RIGHT_EDGE_QRD) < RIGHT_EDGE_THRESH) {
            Pivot(1, 1);
          }
          ZeroTurn(0, 100);
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
          LCD.clear();
          LCD.home();
          LCD.print("CHEWIE STATE");
          statecontrol = State_Chewbacca;
        }
        delay(50);
      } break;
    case State_Chewbacca: {
        DriveStraight(1);
        ChewbaccaDetect();
      } break;
    case State_Zipline2: {
        while (digitalRead(SCISSOR_BUMP)) {
          motor.speed(SCISSOR_MOTOR, 220);
        }
        motor.stop(SCISSOR_MOTOR);
        ReverseStraight(500);
        motor.stop(LEFT_MOTOR);
        motor.stop(RIGHT_MOTOR);
        timerbegin = millis();
        while (millis() - timerbegin < 8000) {
          motor.speed(SCISSOR_MOTOR, -220);
        }
        motor.stop(SCISSOR_MOTOR);
        delay(2000);
        ReverseStraight(1000);
        while (true) {
          motor.stop(SCISSOR_MOTOR);
        }
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
        LCD.print(digitalRead(SCISSOR_BUMP));
        delay(500);
      } break;
    case State_Testing6: {
        LCD.print("LS:");
        LCD.print(analogRead(LEFT_EDGE_QRD));
        LCD.setCursor(8, 0);
        LCD.print("RS:");
        LCD.print(analogRead(RIGHT_EDGE_QRD));
        boolean left = analogRead(LEFT_EDGE_QRD) > LEFT_EDGE_THRESH;
        boolean right = analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH;
        if (!left && !right)
        {
          leftSpeed = MOTOR_BASE_LEFT * 7 / 8;
          rightSpeed = MOTOR_BASE_RIGHT * 8 / 8;
        }
        else if (left && !right)
        {
          leftSpeed = MOTOR_BASE_LEFT * 7 / 4;
          rightSpeed = MOTOR_BASE_RIGHT * 1 / 4;
        }
        else if (!left && right)
        {
          leftSpeed = MOTOR_BASE_LEFT * 1 / 4;
          rightSpeed = MOTOR_BASE_RIGHT * 7 / 4;
        }
        else
        {
          motor.stop(LEFT_MOTOR);
          motor.stop(RIGHT_MOTOR);
        }
        if (rightSpeed > MotorMax.Value)
        {
          rightSpeed = MotorMax.Value;
        }
        else if (rightSpeed < 0)
        {
          rightSpeed = 0;
        }
        if (leftSpeed > MotorMax.Value)
        {
          leftSpeed = MotorMax.Value;
        }
        else if (leftSpeed < 0)
        {
          leftSpeed = 0;
        }
        LCD.setCursor(0, 1);
        motor.speed(LEFT_MOTOR, leftSpeed);
        motor.speed(RIGHT_MOTOR, rightSpeed);
        LCD.print("LM:");
        LCD.print(leftSpeed);
        LCD.setCursor(8, 1);
        LCD.print("RM:");
        LCD.print(rightSpeed);
      } break;
  }
}
