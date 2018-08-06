// Follows tape using PID control
void TapeFollow()
{
  LCD.print("LS:");
  LCD.print(analogRead(LEFT_LF_QRD));
  LCD.setCursor(8, 0);
  LCD.print("RS:");
  LCD.print(analogRead(RIGHT_LF_QRD));
  boolean left = analogRead(LEFT_LF_QRD) > ThreshTape.Value;
  boolean right = analogRead(RIGHT_LF_QRD) > ThreshTape.Value;
  if (left && right)
  {
    error = 0;
    offtape = 0;
  }
  else if (left && !right)
  {
    error = -1;
    offtape = 0;
  }
  else if (!left && right)
  {
    error = 1;
    offtape = 0;
  }
  else
  {
    if (!offtape) {
      offtape = 1;
      offtapebegin = millis();
      offtapetimer = 0;
    } else {
      offtapetimer = millis() - offtapebegin;
    }
    if (prevErr > 0)
    {
      if (offtapetimer < 500) {
        error = 5;
      } else {
        FindTape(0);
      }
    }
    else
    {
      if (offtapetimer < 500) {
        error = -5;
      } else {
        FindTape(1);
      }
    }

  }
  updateTapeFollow();
  leftSpeed = MOTOR_BASE_LEFT + Out;
  rightSpeed = MOTOR_BASE_RIGHT - Out;
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

// PID control step
void updateTapeFollow()
{
  unsigned long now = millis();
  double dtime = (double) (now - prevTime);
  errSum += (error * dtime);
  double dErr = (error - prevErr) / dtime;
  Out = ProportionalGain.Value * error + IntegralGain.Value * errSum + DerivativeGain.Value * dErr;
  prevErr = error;
  prevTime = now;
}

// Avoids edges using QRDs
void TapeFollowEdge()
{
  LCD.print("RS:");
  LCD.print(analogRead(RIGHT_EDGE_QRD));
  boolean right = analogRead(RIGHT_EDGE_QRD) > RIGHT_EDGE_THRESH;
  if (!right)
  {
    leftSpeed = MOTOR_BASE_LEFT * 3/4;
    rightSpeed = MOTOR_BASE_RIGHT * 5/4;
  }
  else
  {
    leftSpeed = MOTOR_BASE_LEFT * 5/4;
    rightSpeed = MOTOR_BASE_RIGHT * 3/4;
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




