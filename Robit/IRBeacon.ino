// Detects infrared beacon
void IRBeacon()
{
  motor.stop(LEFT_MOTOR);
  motor.stop(RIGHT_MOTOR);
  delay(500);
  ///*
  //  timerbegin = millis();
  //
  //  if (analogRead(TEN_KHZ_PIN) > TEN_KHZ_THRESH) {
  //    while(analogRead(TEN_KHZ_PIN) > TEN_KHZ_THRESH){
  //      /*
  //      if (millis() - timerbegin > 12000) {
  //        break;
  //      }
  //      */
  //      LCD.print(analogRead(TEN_KHZ_PIN));
  //      motor.stop(LEFT_MOTOR);
  //      motor.stop(RIGHT_MOTOR);
  //      LCD.setCursor(0,1);
  //      LCD.print("10K seen, waiting");
  //    }
  //  }
  //  while(analogRead(TEN_KHZ_PIN) < TEN_KHZ_THRESH) {
  //    LCD.print(analogRead(TEN_KHZ_PIN));
  //    /*
  //    if (millis() - timerbegin > 12000) {
  //      break;
  //    }
  //*/
  //      motor.stop(LEFT_MOTOR);
  //      motor.stop(RIGHT_MOTOR);
  //      LCD.setCursor(0,1);
  //      LCD.print("Signal Low, waiting");
  //  }
  //  */
  if (analogRead(TEN_KHZ_PIN) > TEN_KHZ_THRESH) {
    while (analogRead(TEN_KHZ_PIN) > TEN_KHZ_THRESH) {
      LCD.print(analogRead(ONE_KHZ_PIN));
      LCD.print("/");
      LCD.print(analogRead(TEN_KHZ_PIN));
      LCD.setCursor(0, 1);
      motor.stop(LEFT_MOTOR);
      motor.stop(RIGHT_MOTOR);
      LCD.clear();
      LCD.print("Waiting for 1KHZ");
    }
  }
  while (analogRead(TEN_KHZ_PIN) < TEN_KHZ_THRESH) {
    LCD.print(analogRead(ONE_KHZ_PIN));
    LCD.print("/");
    LCD.print(analogRead(TEN_KHZ_PIN));
    LCD.setCursor(0, 1);
    motor.stop(LEFT_MOTOR);
    motor.stop(RIGHT_MOTOR);
    LCD.clear();
    LCD.print("Waiting for 10KHZ");
  }
  //  while(true){
  //  LCD.clear();
  //  LCD.print(analogRead(ONE_KHZ_PIN));
  //    LCD.print("/");
  //    LCD.print(analogRead(TEN_KHZ_PIN));
  //    LCD.setCursor(0,1);
  //    LCD.print("GO!");
  //  }
  statecontrol = State_Archway;
}


