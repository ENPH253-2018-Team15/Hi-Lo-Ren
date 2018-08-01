#include <Wire_slave.h>

const byte ECHO_CLAW = PC14;
const byte TRIG_CLAW = PC13;
const byte ECHO_LEFT = PA0;
const byte TRIG_LEFT = PC15;
const byte ECHO_RIGHT = PA2;
const byte TRIG_RIGHT = PA1;
const byte STEPPER_CW = PA7;
const byte STEPPER_CCW = PA6;
const byte I2C_ADDR = 2;
const uint16_t clawsteps90 = 500;
const byte steppertime = 10;
byte currclawpos;
byte nextclawpos;
long distance;
enum Mode
{
  Mode_EwokDetect,
  Mode_ZiplineDetect,
  Mode_ClawRotate
};
Mode currentMode;

void setup()
{
  Serial.begin(9600);
  Wire.begin(I2C_ADDR); // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent);
  pinMode(ECHO_CLAW, INPUT);
  pinMode(TRIG_CLAW, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);
  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT);
  currentMode = Mode_EwokDetect;
  currclawpos = 2;
  nextclawpos = 1;
}

void loop()
{
}

// Gets information from the master to know which data to send
void receiveEvent(int howMany)
{
  while (!Wire.available()) {
    delay(1);
  }
  byte received = Wire.read();
  if (received == 0) {
    currentMode = Mode_EwokDetect;
  } else if (received == 1) {
    currentMode = Mode_ZiplineDetect;
  } else if (received == 2) {
    currentMode = Mode_ClawRotate;
  } else if (received == 10) {
    nextclawpos = 0;
  } else if (received == 11) {
    nextclawpos = 1;
  } else if (received == 12) {
    nextclawpos = 2;
  }
}
// Provides information from sensors to the master
void requestEvent()
{
  if (currentMode == Mode_EwokDetect) {
    Wire.write(UltrasonicDistance(TRIG_CLAW, ECHO_CLAW));
    Wire.write(UltrasonicDistance(TRIG_LEFT, ECHO_LEFT));
    Wire.write(UltrasonicDistance(TRIG_RIGHT, ECHO_RIGHT));
  } else if (currentMode == Mode_ZiplineDetect) {
  } else if (currentMode == Mode_ClawRotate) {
    byte turn = nextclawpos - currclawpos;
    if (turn > 0){
      digitalWrite(STEPPER_CCW,0);
      for (uint16_t i = 0; i < turn * clawsteps90; i++){
        digitalWrite(STEPPER_CW,1);
        delay(steppertime);
        digitalWrite(STEPPER_CW,0);
        delay(steppertime);
      }
    } else if (turn < 0){
      digitalWrite(STEPPER_CW,0);
      for (uint16_t i = 0; i < -turn * clawsteps90; i++){
        digitalWrite(STEPPER_CCW,1);
        delay(steppertime);
        digitalWrite(STEPPER_CCW,0);
        delay(steppertime);
      }
    }
    Wire.write(1);
  }
}

// Calculates the distance of object in front of ultrasonic sensor
uint8_t UltrasonicDistance(byte trig, byte echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  distance = pulseIn(echo, HIGH) * .034 / 2;
  if (distance > 255) {
    distance = 255;
  }
  return distance;
}
