#include <Wire_slave.h>
#include <Servo.h>

const byte ECHO_CLAW = PC14;
const byte TRIG_CLAW = PC13;
const byte ECHO_LEFT = PA0;
const byte TRIG_LEFT = PC15;
const byte ECHO_RIGHT = PA2;
const byte TRIG_RIGHT = PA1;
enum Mode
{
  Mode_EwokDetect,
  Mode_ZiplineDetect
}
Mode currentMode;

long distance;

void setup()
{
  Serial.begin(9600);
  Wire.begin(2); // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent);
  pinMode(ECHO_CLAW, INPUT);
  pinMode(TRIG_CLAW, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);
  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT);
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


