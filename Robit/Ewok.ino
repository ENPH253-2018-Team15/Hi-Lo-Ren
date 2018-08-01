void EwokDetect()
{
  /*
	// Reading ultrasonic sensors connected to bluepill
	Wire.beginTransmission(BLUE_ADDR1);
	Wire.write(0);
	Wire.endTransmission();
	uint8_t dists[3];
	Wire.requestFrom(BLUE_ADDR1, 3);
	while (!Wire.available())
	{
		delay(1);

		
	}
	for (int i = 0; i < 3; i++)
	{
		uint8_t d = Wire.read();
		// LCD.print(d);
		// LCD.print("/");
		dists[i] = d;
	}
 */
  digitalWrite(35, LOW);
  delayMicroseconds(2);
  digitalWrite(35, HIGH);
  delayMicroseconds(10);
  digitalWrite(35, LOW);
  uint16_t distance = pulseIn(14, HIGH) * .034 / 2;
  if (distance > 255) {
    distance = 255;
  }
  if (distance < 10 && distance !=0){
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
    LCD.clear();
    LCD.print("EWOK DETECTED");
    LCD.setCursor(0,1);
    LCD.print(distance);
    delay(1000);
    DriveStraight(testtime1);
    FindTape(0);
    statecontrol = State_Bridge1Align;
  }
}

// If object detected near Ewok location, go to Ewok Retrieval state
void EwokRetrieve()
{
	// Align robot to Ewok
	// Pick up Ewok
}













