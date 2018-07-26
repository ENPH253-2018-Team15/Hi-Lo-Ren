// Align to Zipline based on ultrasonic sensor
void ZipAlign()
{
}

// Lift box with scissor lift
void ScissorLift(boolean direction)
{
  if (direction) {
    if (digitalRead(SCISSOR_BUMP)) {
      motor.stop(SCISSOR_MOTOR);
    } else {
      motor.speed(SCISSOR_MOTOR, 255);
    }
  } else {
    motor.speed(SCISSOR_MOTOR, -255);
    delay(5000);
    motor.stop(SCISSOR_MOTOR);
  }
}

// Place box on zipline
void ZiplinePlace()
{
}

// Detecting zipline above
void ZiplineDetect()
{
  // Reading ultrasonic sensors connected to bluepill
  Wire.beginTransmission(BLUE_ADDR1);
  Wire.write(1);
  Wire.endTransmission();
  uint8_t dists[2];
  Wire.requestFrom(BLUE_ADDR1, 2);
  while (!Wire.available())
  {
    delay(1);
  }
  for (int i = 0; i < 2; i++)
  {
    uint8_t d = Wire.read();
    // LCD.print(d);
    // LCD.print("/");
    dists[i] = d;
  }
}





