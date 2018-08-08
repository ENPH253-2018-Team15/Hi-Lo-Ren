void EdgeFollow()
{
  LCD.print("LS:");
  LCD.print(analogRead(LEFT_EDGE_QRD));
  LCD.setCursor(8, 0);
  LCD.print("RS:");
  LCD.print(analogRead(RIGHT_EDGE_QRD));
  boolean left = analogRead(LEFT_EDGE_QRD) > LEFT_EDGE_THRESH;
  boolean right = analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH;
  if (!left && !right)
  {
    leftSpeed = MOTOR_BASE_LEFT;
    rightSpeed = MOTOR_BASE_RIGHT * 2 / 4;
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
}

// Avoids edges using QRDs
void EdgeAvoid()
{
  LCD.print("LS:");
  LCD.print(analogRead(LEFT_EDGE_QRD));
  LCD.setCursor(8, 0);
  LCD.print("RS:");
  LCD.print(analogRead(RIGHT_EDGE_QRD));
  boolean left = analogRead(LEFT_EDGE_QRD) > LEFT_EDGE_THRESH;
  boolean right = analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH;
  if (!left && !right)
  {
    leftSpeed = MOTOR_BASE_LEFT;
    rightSpeed = MOTOR_BASE_RIGHT* 7 / 8;
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
}
