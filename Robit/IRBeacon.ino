// Detects infrared beacon
void IRBeacon()
{
  tenkhzread = analogRead(TEN_KHZ_PIN);
  onekhzread = analogRead(ONE_KHZ_PIN);
  LCD.print("1KHZ:");
  LCD.print(onekhzread);
  LCD.setCursor(0, 1);
  LCD.print("10KHZ:");
  LCD.print(tenkhzread);
  // if 1kHz detected, wait until 10kHz detected
}