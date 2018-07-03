void TapeFollow()
{
  LCD.print("LS:");
  LCD.print(analogRead(LEFT_LF_QRD));
  LCD.setCursor(8, 0);
  LCD.print("RS:");
  LCD.print(analogRead(RIGHT_LF_QRD));

  if (analogRead(RIGHT_LF_QRD) > 100)
  {
    right = 1;
  }
  else
  {
    right = 0;
  }
  if (analogRead(LEFT_LF_QRD) > 100)
  {
    left = 1;
  }
  else
  {
    left = 0;
  }
  if (left && right)
  {
    error = 0;
  }
  else if (left && !right)
  {
    error = -1;
  }
  else if (!left && right)
  {
    error = 1;
  }
  else
  {
    if (prevErr > 0)
    {
      error = 5;
    }
    else
    {
      error = -5;
    }
  }
  update();
  leftSpeed = MotorBase.Value + Out;
  rightSpeed = MotorBase.Value - Out;
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
}

void update()
{
  unsigned long now = millis();
  double dtime = (double)(now - prevTime);

  errSum += (error * dtime);
  double dErr = (error - prevErr) / dtime;

  Out = ProportionalGain.Value * error + IntegralGain.Value * errSum + DerivativeGain.Value * dErr;

  prevErr = error;
  prevTime = now;
}
