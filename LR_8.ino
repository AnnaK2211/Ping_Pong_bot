#include <AFMotor.h>

const int DIRECTION_PIN = 2;
const int STOP_PIN = A0;

const int MOTOR_SPEED = 100;
const int MOTOR_COUNT = 4;

AF_DCMotor motors[MOTOR_COUNT] = {AF_DCMotor(1), AF_DCMotor(2), AF_DCMotor(3), AF_DCMotor(4)};

void runMotors(int direction) {
  for (int i = 0; i < MOTOR_COUNT; i++) {
    motors[i].run(direction);
  }
}

void setup() {
  for (int i = 0; i < MOTOR_COUNT; i++) {
    motors[i].setSpeed(MOTOR_SPEED);
    motors[i].run(RELEASE);
  }

  pinMode(DIRECTION_PIN, INPUT);
  pinMode(STOP_PIN, INPUT);
}

void loop() {
  if (digitalRead(STOP_PIN) == HIGH) {
    runMotors(RELEASE);
  } 
  else {
    if (digitalRead(DIRECTION_PIN) == HIGH) {
      runMotors(FORWARD);
    } else {
      runMotors(BACKWARD);
    }
  }
}