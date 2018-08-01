void Menu()
{
  timer = millis() - timerbegin;
  LCD.clear();
  LCD.home();
  LCD.print("Entering menu");
  delay(500);
  motor.speed(LEFT_MOTOR, 0);
  motor.speed(RIGHT_MOTOR, 0);
  while (true)
  {
    /* Show MenuItem value and knob value */
    int menuIndex = knob(6) * (MenuItem::MenuItemCount) / 1024;
    LCD.clear();
    LCD.home();
    LCD.print(menuItems[menuIndex].Name);
    LCD.print(" ");
    LCD.print(menuItems[menuIndex].Value);
    LCD.setCursor(0, 1);
    LCD.print("Set to ");
    LCD.print(knob(7) / 4);
    LCD.print("?");
    delay(100);
    /* Press start button to save the new value */
    if (menuItems[menuIndex].Name.equals("RunScissor")) {
      if (startbutton()) {
        delay(100);
        while (startbutton()) {
          motor.speed(SCISSOR_MOTOR, 255);
        }
        motor.stop(SCISSOR_MOTOR);
      } else if (stopbutton()) {
        delay(100);
        while (stopbutton()) {
          motor.speed(SCISSOR_MOTOR, -255);
        }
        motor.stop(SCISSOR_MOTOR);
      }
    } else if (menuItems[menuIndex].Name.equals("RunLeft")) {
      if (startbutton()) {
        delay(100);
        while (startbutton()) {
          motor.speed(LEFT_MOTOR, 255);
        }
        motor.stop(LEFT_MOTOR);
      } else if (stopbutton()) {
        delay(100);
        while (stopbutton()) {
          motor.speed(LEFT_MOTOR, -255);
        }
        motor.stop(LEFT_MOTOR);
      }
    } else if (menuItems[menuIndex].Name.equals("RunRight")) {
      if (startbutton()) {
        delay(100);
        while (startbutton()) {
          motor.speed(RIGHT_MOTOR, 255);
        }
        motor.stop(RIGHT_MOTOR);
      } else if (stopbutton()) {
        delay(100);
        while (stopbutton()) {
          motor.speed(RIGHT_MOTOR, -255);
        }
        motor.stop(RIGHT_MOTOR);
      }
    } else if (menuItems[menuIndex].Name.equals("RunTimer")) {
      if (startbutton()) {
        delay(100);
        while (startbutton()) {
          LCD.print("T");
          LCD.print(timer);
        }
      }
    } else if (menuItems[menuIndex].Name.equals("RunServo0")) {
      if (startbutton()) {
        delay(100);
        if (startbutton()) {
          RCServo0.write(90);
        }
      } else if (stopbutton()) {
        delay(100);
        if (stopbutton()) {
          RCServo0.write(0);
        }
      }
    } else if (menuItems[menuIndex].Name.equals("RunServo1")) {
      if (startbutton()) {
        delay(100);
        if (startbutton()) {
          RCServo1.write(90);
        }
      } else if (stopbutton()) {
        delay(100);
        if (stopbutton()) {
          RCServo1.write(0);
        }
      }
    } else if (menuItems[menuIndex].Name.equals("RunServo2")) {
      if (startbutton()) {
        delay(100);
        if (startbutton()) {
          RCServo2.write(90);
        }
      } else if (stopbutton()) {
        delay(100);
        if (stopbutton()) {
          RCServo2.write(0);
        }
      }
    } else if (menuItems[menuIndex].Name.equals("RunClaw")) {
      if (startbutton()) {
        delay(100);
        while (startbutton()) {
          motor.speed(CLAW_MOTOR, 255);
        }
        motor.stop(CLAW_MOTOR);
      } else if (stopbutton()) {
        delay(100);
        while (stopbutton()) {
          motor.speed(CLAW_MOTOR, -255);
        }
        motor.stop(CLAW_MOTOR);
      }
    } else if (menuItems[menuIndex].Name.equals("StartState")) {
      if (startbutton()) {
        delay(100);
        if (startbutton())
        {
          menuItems[menuIndex].Value = knob(7) / 16;
          menuItems[menuIndex].Save();
          delay(250);
        }
      }
    } 
    else {
      if (startbutton())
      {
        delay(100);
        if (startbutton())
        {
          menuItems[menuIndex].Value = knob(7) / 4;
          menuItems[menuIndex].Save();
          delay(250);
        }
      }
      /* Press stop button to exit menu */
      if (stopbutton())
      {
        delay(100);
        if (stopbutton())
        {
          LCD.clear();
          LCD.home();
          LCD.print("Leaving menu");
          delay(500);
          timerbegin = millis();
          return;
        }
      }
    }
  }
}

