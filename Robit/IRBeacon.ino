// Detects infrared beacon
void IRBeacon()
{
	tenkhzread = analogRead(TEN_KHZ_PIN) > 500;
	onekhzread = analogRead(ONE_KHZ_PIN) > 500;
	LCD.print("1:");
	LCD.print(analogRead(ONE_KHZ_PIN));
  LCD.print("/");
	LCD.print("10:");
	LCD.print(analogRead(TEN_KHZ_PIN));
  LCD.setCursor(0,1);
  if (!onekhzread && !tenkhzread) {
    LCD.print("NO IR DETECTED");
  } else if (onekhzread && !tenkhzread){
    LCD.print("1KHZ DETECTED");
  } else if (tenkhzread){
    LCD.print("10KHZ DETECTED");
  }
	// if 1kHz detected, wait until 10kHz detected
}

