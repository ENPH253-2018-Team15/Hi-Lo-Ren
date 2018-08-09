boolean ArchDetect()
{
    digitalWrite(LEFT_US_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(LEFT_US_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(LEFT_US_TRIG, LOW);
    uint16_t dist = pulseIn(LEFT_US_ECHO, HIGH) * .034 / 2;
    LCD.clear();
    LCD.print(dist);
    if (dist> 80) {
      return true;
    } else{
      return false;
    }
}
