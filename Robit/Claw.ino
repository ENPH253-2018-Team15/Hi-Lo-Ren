void ClawRotate(int8_t newpos) {
  if (newpos == -1) {
    if (analogRead(CLAW_POT) > CLAW_LEFT) {
      while (analogRead(CLAW_POT) > CLAW_LEFT) {
        motor.speed(CLAW_MOTOR, -CLAW_SPEED);
        ClawServo();
      }
      motor.stop(CLAW_MOTOR);
    }
  } else if (newpos == 0) {
    if (analogRead(CLAW_POT) > CLAW_UP_RIGHT) {
      while (analogRead(CLAW_POT) > CLAW_UP_RIGHT) {
        motor.speed(CLAW_MOTOR, -CLAW_SPEED);
        ClawServo();
      }
    } else if (analogRead(CLAW_POT) < CLAW_UP_LEFT) {
      while (analogRead(CLAW_POT) < CLAW_UP_LEFT) {
        motor.speed(CLAW_MOTOR, CLAW_SPEED);
        ClawServo();
      }
    }
    motor.stop(CLAW_MOTOR);

  } else if (newpos == 1) {
    if (analogRead(CLAW_POT) < CLAW_RIGHT) {
      while (analogRead(CLAW_POT) < CLAW_RIGHT) {
        motor.speed(CLAW_MOTOR, CLAW_SPEED);
        ClawServo();
      }
    }
    motor.stop(CLAW_MOTOR);
  }
  RCServo0.write(CLAW_SERVO_OPEN);
}

void ClawServo() {
  if (analogRead(CLAW_POT) > CLAW_UP_LEFT && analogRead(CLAW_POT) < CLAW_UP_RIGHT) {
    RCServo0.write(CLAW_SERVO_OPEN);
  } else if (analogRead(CLAW_POT) < CLAW_UP_LEFT || analogRead(CLAW_POT) > CLAW_UP_RIGHT) {
    RCServo0.write(CLAW_SERVO_CLOSED);
  }
}



