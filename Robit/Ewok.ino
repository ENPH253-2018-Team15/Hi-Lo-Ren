void EwokDetect()
{
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
}

// If object detected near Ewok location, go to Ewok Retrieval state
void EwokRetrieve()
{
	// Align robot to Ewok
	// Pick up Ewok
}




