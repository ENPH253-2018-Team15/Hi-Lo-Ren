// Turn about centre of axle
void ZeroTurn(boolean direction, unsigned long duration)
{
	if (!direction)
	{
		motor.speed(LEFT_MOTOR, -MotorBase.Value);
		motor.speed(RIGHT_MOTOR, MotorBase.Value);
	}
	else
	{
		motor.speed(RIGHT_MOTOR, -MotorBase.Value);
		motor.speed(LEFT_MOTOR, MotorBase.Value);
	}
	delay(duration);
}

// Pivot about wheel
void Pivot(boolean direction, unsigned long duration)
{
	if (!direction)
	{
		motor.stop(LEFT_MOTOR);
		motor.speed(RIGHT_MOTOR, MotorBase.Value);
	}
	else
	{
		motor.stop(RIGHT_MOTOR);
		motor.speed(LEFT_MOTOR, MotorBase.Value);
	}
	delay(duration);
}

void DriveStraight(unsigned long duration)
{
  motor.speed(LEFT_MOTOR, MotorBase.Value);
  motor.speed(RIGHT_MOTOR, MotorBase.Value);
  delay(duration);
}

void ReverseStraight(unsigned long duration)
{
  motor.speed(LEFT_MOTOR, -MotorBase.Value);
  motor.speed(RIGHT_MOTOR, -MotorBase.Value);
  delay(duration);
}

// Celebration dance
void Dance()
{
}





