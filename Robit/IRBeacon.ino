// Detects infrared beacon
void IRBeacon()
{
  motor.stop(LEFT_MOTOR);
  motor.stop(RIGHT_MOTOR);
  tenkhzread = analogRead(TEN_KHZ_PIN) > 500;
  onekhzread = analogRead(ONE_KHZ_PIN) > 500;
  if (analogRead(TEN_KHZ_PIN) < 500 && analogRead(ONE_KHZ_PIN) < 500) {
    LCD.clear();
    LCD.print("NO IR FOUND");
    while (analogRead(TEN_KHZ_PIN) < 500 && analogRead(ONE_KHZ_PIN) < 500) {
      Pivot(1, 1);
    }
  }
  while (analogRead(TEN_KHZ_PIN) < 500 && analogRead(ONE_KHZ_PIN) > 500) {
    LCD.clear();
    LCD.print("1KHZ FOUND");
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
  }
  LCD.clear();
  LCD.print("10KHZ FOUND");
  ClawRotate(0);
}


