boolean EwokDetected() {
  if (analogRead(EWOK_DETECTOR) < 10) {
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
    return true;
  } else {
    return false;
  }
}

void Ewok1Detect()
{
  if (EwokDetected()) {
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
    MOTOR_BASE_LEFT = MotorBase.Value + LeftMotorOffset.Value;
    MOTOR_BASE_RIGHT = MotorBase.Value + RightMotorOffset.Value;
    LCD.clear();
    LCD.print("EWOK 1 DETECTED");
    RCServo0.write(CLAW_SERVO_CLOSED);
    ClawRotate(0);
    PivotBack(0, 500);
    statecontrol = State_EdgeAlign1;

    /*
      while(analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH){
      Pivot(0,1);
      }
      statecontrol = State_Bridge1Place;
    */
  }
}

void Ewok2Detect()
{
  if (EwokDetected()) {
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
    LCD.clear();
    LCD.print("EWOK 2 DETECTED");
    RCServo0.write(CLAW_SERVO_CLOSED);
    ClawRotate(0);
    ZeroTurn(0, 50);
    ReverseStraight(500);
    ZeroTurn(1, 500);
    while (analogRead(LEFT_EDGE_QRD) < LEFT_EDGE_THRESH) {
      DriveStraight(1);
    }
    while (analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH) {
      Pivot(0, 1);
    }
    IRBeacon();
    statecontrol = State_Ewok3;
  }
}

void Ewok3Detect()
{
  if (EwokDetected()) {
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
    delay(500);
    LCD.clear();
    LCD.print("EWOK3 DETECTED");
    RCServo0.write(CLAW_SERVO_CLOSED);
    delay(500);
    ClawRotate(0);
    FindTape(0, 500);
    while (analogRead(LEFT_EDGE_QRD) < LEFT_EDGE_THRESH && analogRead(RIGHT_EDGE_QRD) < RIGHT_EDGE_THRESH) {
      TapeFollow();
    }
    MOTOR_BASE_LEFT = MotorBase.Value + LeftMotorOffset.Value;
    MOTOR_BASE_RIGHT = MotorBase.Value + RightMotorOffset.Value;
    statecontrol = State_Edge2Align1;
  }
}

void Ewok4Detect()
{
  if (EwokDetected()) {
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
    LCD.clear();
    LCD.print("EWOK4 DETECTED");
    RCServo0.write(CLAW_SERVO_CLOSED);
    ClawRotate(0);
    statecontrol = State_Chewbacca;
  }
}
void ChewbaccaDetect()
{
  if (EwokDetected()) {
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
    LCD.clear();
    LCD.print("CHEWIE DETECTED");
    RCServo0.write(CLAW_SERVO_CLOSED);
    delay(500);
    ClawRotate(0);
    RCServo0.write(CLAW_SERVO_OPEN);
    delay(1000);
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
    ClawRotate(-1);
    while (analogRead(RIGHT_EDGE_QRD) < RIGHT_EDGE_THRESH) {
      Pivot(1, 1);
    }
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
    RCServo1.write(BRIDGE1_SERVO_OPEN);
    RCServo2.write(BRIDGE2_SERVO_OPEN);
    statecontrol = State_Zipline2;
  }
}
