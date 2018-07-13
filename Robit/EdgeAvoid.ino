// Avoids edges using QRDs
void EdgeAvoid()
{
  LCD.print("LS:");
  LCD.print(analogRead(LEFT_EDGE_QRD));
  LCD.setCursor(8, 0);
  LCD.print("RS:");
  LCD.print(analogRead(RIGHT_EDGE_QRD));
  if (!left && !right)
  {
    Out = 0;
  }
  else
    if (left && !right)
    {
      Out = -50;
    }
  else
    if (!left && right)
    {
      Out = 50;
    }
  else
  {
    motor.stop_all();
  }
  leftSpeed = MotorBase.Value + Out;
  rightSpeed = MotorBase.Value - Out;
  if (rightSpeed > MotorMax.Value)
  {
    rightSpeed = MotorMax.Value;
  }
  else
    if (rightSpeed < 0)
    {
      rightSpeed = 0;
    }
  if (leftSpeed > MotorMax.Value)
  {
    leftSpeed = MotorMax.Value;
  }
  else
    if (leftSpeed < 0)
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



