// Detects infrared beacon
void IRBeacon()
{
  motor.stop(LEFT_MOTOR);
  motor.stop(RIGHT_MOTOR);


  if (analogRead(TEN_KHZ_PIN) > TEN_KHZ_THRESH) {
    while(analogRead(TEN_KHZ_PIN) > TEN_KHZ_THRESH){
      LCD.print(analogRead(TEN_KHZ_PIN));
      motor.stop(LEFT_MOTOR);
      motor.stop(RIGHT_MOTOR);
      LCD.setCursor(0,1);
      LCD.print("10K seen, waiting");
    }
  }
  while(analogRead(TEN_KHZ_PIN) < TEN_KHZ_THRESH) {
    LCD.print(analogRead(TEN_KHZ_PIN));
      motor.stop(LEFT_MOTOR);
      motor.stop(RIGHT_MOTOR);
      LCD.setCursor(0,1);
      LCD.print("Signal Low, waiting");
  } 
//  while(analogRead(ONE_KHZ_PIN) < ONE_KHZ_THRESH){
//    LCD.print(analogRead(ONE_KHZ_PIN));
//    LCD.print("/");
//    LCD.print(analogRead(TEN_KHZ_PIN));
//    LCD.setCursor(0,1);
//    motor.stop(LEFT_MOTOR);
//    motor.stop(RIGHT_MOTOR);
//    LCD.clear();
//    LCD.print("Waiting for 1KHZ");
//  }
//  while(analogRead(TEN_KHZ_PIN) < TEN_KHZ_THRESH || analogRead(ONE_KHZ_PIN) > ONE_KHZ_THRESH){
//    LCD.print(analogRead(ONE_KHZ_PIN));
//    LCD.print("/");
//    LCD.print(analogRead(TEN_KHZ_PIN));
//    LCD.setCursor(0,1);
//    motor.stop(LEFT_MOTOR);
//    motor.stop(RIGHT_MOTOR);
//    LCD.clear();
//    LCD.print("Waiting for 10KHZ");
//  }
  statecontrol = State_Archway;
}


