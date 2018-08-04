// Turn about centre of axle
void ZeroTurn(boolean direction, unsigned long duration)
{
  if (!direction)
  {
    motor.speed(LEFT_MOTOR, -MOTOR_BASE);
    motor.speed(RIGHT_MOTOR, MOTOR_BASE);
  }
  else
  {
    motor.speed(RIGHT_MOTOR, -MOTOR_BASE);
    motor.speed(LEFT_MOTOR, MOTOR_BASE);
  }
  delay(duration);
}

// Pivot about wheel
void Pivot(boolean direction, unsigned long duration)
{
  if (!direction)
  {
    motor.stop(LEFT_MOTOR);
    motor.speed(RIGHT_MOTOR, MOTOR_BASE);
  }
  else
  {
    motor.stop(RIGHT_MOTOR);
    motor.speed(LEFT_MOTOR, MOTOR_BASE);
  }
  delay(duration);
}

// Pivot about wheel
void PivotBack(boolean direction, unsigned long duration)
{
  if (!direction)
  {
    motor.stop(LEFT_MOTOR);
    motor.speed(RIGHT_MOTOR, -MOTOR_BASE);
  }
  else
  {
    motor.stop(RIGHT_MOTOR);
    motor.speed(LEFT_MOTOR, -MOTOR_BASE);
  }
  delay(duration);
}

void DriveStraight(unsigned long duration)
{
  motor.speed(LEFT_MOTOR, MOTOR_BASE);
  motor.speed(RIGHT_MOTOR, MOTOR_BASE);
  delay(duration);
}

void ReverseStraight(unsigned long duration)
{
  motor.speed(LEFT_MOTOR, -MOTOR_BASE);
  motor.speed(RIGHT_MOTOR, -MOTOR_BASE);
  delay(duration);
}

void FindTape(boolean direction) {
  uint32_t findtapebegin = millis();
  boolean switched = 0;
  while (analogRead(LEFT_LF_QRD) < ThreshTape.Value && analogRead(RIGHT_LF_QRD) < ThreshTape.Value) {
    if (millis() - findtapebegin > 3000 && !switched) {
      direction = !direction;
      switched = 1;
    }
    if (direction) {
      ZeroTurn(1, 1);
    } else {
      ZeroTurn(0, 1);
    }
  }
}

// Celebration dance
void Dance()
{
  ZeroTurn(0, 200);
  ZeroTurn(1, 200);
}








