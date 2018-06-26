#include <phys253.h>
#include <LiquidCrystal.h>

#define leftMax 255
#define rightMax 255
#define leftBase 175
#define rightBase 175
double Kp = 0;
double Ki = 0;
double Kd = 0;
const byte leftMotor = 0;
const byte rightMotor = 1;
const byte leftSensor = 4; //analog3
const byte rightSensor = 5; //analog2
int leftSpeed = 0;
int rightSpeed = 0;
double error = 0;
unsigned long prevTime;
double In, Out;
double errSum, prevErr;
boolean left, right;
boolean show = 0;

void setup()
{
#include <phys253setup.txt>
  Serial.begin(9600) ;
  pinMode(leftMotor,OUTPUT);
  pinMode(rightMotor,OUTPUT);
  pinMode(leftSensor,INPUT);
  pinMode(rightSensor,INPUT);
  pinMode(6,INPUT);
  pinMode(7,INPUT);
}

void loop()
{
  LCD.clear();
  LCD.home();
  Kp = ((double)knob(6))/10;
  Kd = ((double)knob(7))/10;
  if (startbutton()){
    show = 1;
  } else if (stopbutton()){
    show = 0;
  }
  if (show){
    LCD.print(Kp);
    LCD.print(",");
    LCD.print(Ki);
    LCD.print(",");
    LCD.print(Kd);
  }else{
  LCD.print("LS:");
  LCD.print(analogRead(leftSensor));
  LCD.setCursor(8,0);
  LCD.print("RS:");
  LCD.print(analogRead(rightSensor));
  }
  if (analogRead(rightSensor) >100){
    right = 1;
  } else{
    right = 0;
  }
  if (analogRead(leftSensor)>100){
    left = 1;
  } else {
    left = 0;
  }
  if (left && right){
    In = 0;
  } else if (left && !right){
    In = -1;
  } else if (!left && right){
    In = 1;
  } else {
    if (prevErr>0){
      In = 5;
    } else {
      In = -5;
    }
  }
  update();
  leftSpeed = leftBase + Out;
  rightSpeed = rightBase - Out;
  if (rightSpeed>rightMax){
    rightSpeed = rightMax;
  } else if (rightSpeed<0){
    rightSpeed = 0;
  }
  if (leftSpeed>leftMax){
    leftSpeed = leftMax;
  } else if (leftSpeed<0){
    leftSpeed = 0;
  }
  LCD.setCursor(0,1);
  motor.speed(leftMotor,leftSpeed);
  motor.speed(rightMotor,rightSpeed);
  LCD.print("LM:");
  LCD.print(leftSpeed);
  LCD.setCursor(8,1);
  LCD.print("RM:");
  LCD.print(rightSpeed);
}

void update(){
  unsigned long now = millis();
  double dtime = (double)(now-prevTime);

  double error = In;
  errSum += (error * dtime);
  double dErr = (error - prevErr)/dtime;

  Out = Kp * error + Ki * errSum + Kd * dErr;

  prevErr = error;
  prevTime = now;
}

