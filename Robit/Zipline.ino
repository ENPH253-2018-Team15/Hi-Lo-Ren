// Align to Zipline based on ultrasonic sensor
void ZipAlign()
{
}

// Lift box with scissor lift
void ScissorLift(boolean direction)
{
  if (direction) {
    if (!digitalRead(SCISSOR_BUMP)) {
      motor.stop(SCISSOR_MOTOR);
    } else {
      motor.speed(SCISSOR_MOTOR, 255);
    }
  } else {
    motor.speed(SCISSOR_MOTOR, -255);
    delay(5000);
    motor.stop(SCISSOR_MOTOR);
  }
}

// Place box on zipline
void ZiplinePlace()
{
}

// Detecting zipline above
void Zipline1Detect()
{
  digitalWrite(LEFT_US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(LEFT_US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(LEFT_US_TRIG, LOW);
  uint16_t leftdist = pulseIn(LEFT_US_ECHO, HIGH) * .034 / 2;
  digitalWrite(RIGHT_US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(RIGHT_US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(RIGHT_US_TRIG, LOW);
  uint16_t rightdist = pulseIn(RIGHT_US_ECHO, HIGH) * .034 / 2;
  boolean leftfound = leftdist < 15 && leftdist != 0;
  boolean rightfound = rightdist < 15 && rightdist != 0;
  if (leftfound && rightfound) {
    ZeroTurn(1, 200);
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
    statecontrol = State_Bridge2Align;
  } else if (leftfound) {
    PivotBack(0, 1);
  } else if (rightfound) {
    PivotBack(1, 1);
  } else {
    ReverseStraight(1);
  }
}

void Zipline2Detect()
{
  digitalWrite(LEFT_US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(LEFT_US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(LEFT_US_TRIG, LOW);
  uint16_t leftdist = pulseIn(LEFT_US_ECHO, HIGH) * .034 / 2;
  digitalWrite(RIGHT_US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(RIGHT_US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(RIGHT_US_TRIG, LOW);
  uint16_t rightdist = pulseIn(RIGHT_US_ECHO, HIGH) * .034 / 2;
  boolean leftfound = leftdist < 15 && leftdist != 0;
  boolean rightfound = rightdist < 15 && rightdist != 0;
  if (leftfound && rightfound) {
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
    ScissorLift(1);
    delay(1000);
    ReverseStraight(1000);
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
    delay(1000);
    ScissorLift(0);
  } else if (leftfound) {
    Pivot(0, 1);
  } else if (rightfound) {
    Pivot(1, 1);
  } else {
    DriveStraight(1);
  }
}

boolean ZiplineDetect(boolean direction){
  uint16_t dist;
  if (direction){
  digitalWrite(RIGHT_US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(RIGHT_US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(RIGHT_US_TRIG, LOW);
  dist = pulseIn(RIGHT_US_ECHO, HIGH) * .034 / 2;
  } else{
  digitalWrite(LEFT_US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(LEFT_US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(LEFT_US_TRIG, LOW);
  dist = pulseIn(LEFT_US_ECHO, HIGH) * .034 / 2;
  }
  return dist < 15 && dist != 0;
}








