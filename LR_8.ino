#include <AFMotor.h>

const int DIRECTION_PIN = 2;
const int STOP_PIN = A0;

const int MOTOR_SPEED = 100;
const int MOTOR_COUNT = 4;
const int ACCEL_DECEL_TIME = 1000;

AF_DCMotor motors[MOTOR_COUNT] = {AF_DCMotor(1), AF_DCMotor(2), AF_DCMotor(3), AF_DCMotor(4)};

enum Direction {
  AHEAD,
  BACK,
  STOP
};

Direction directionState = STOP;
Direction previousDirectionState = STOP;

class Timer {
  private:
    unsigned long duration;
    unsigned long timerStart;

  public:
    Timer(unsigned long timerDuration) {
      duration = timerDuration;
      timerStart = 0;
    }

    void restart() {
      timerStart = millis();
    }

    bool isElapsed() {
      return millis() - timerStart >= duration;
    }
};

void setMotorsSpeed(int speed) {
  for (int i = 0; i < MOTOR_COUNT; i++) {
    motors[i].setSpeed(speed);
  }
}

void runMotors(Direction state) {
  for (int i = 0; i < MOTOR_COUNT; i++) {
    switch (state) {
      case STOP:
        motors[i].run(RELEASE);
        break;
      case AHEAD:
        motors[i].run(FORWARD);
        break;
      case BACK:
        motors[i].run(BACKWARD);
        break;
    }
  }
}

void brake(int duration) {
  Timer timer(duration / MOTOR_SPEED);
  timer.restart();
  
  for (int currentSpeed = MOTOR_SPEED; currentSpeed >= 0; ) {
    setMotorsSpeed(currentSpeed);
    if (timer.isElapsed()) {
      currentSpeed--;
      timer.restart();
    }
  }

  runMotors(STOP);
}

void accelerate(Direction newDirection, int duration) {
  runMotors(newDirection);

  Timer timer(duration / MOTOR_SPEED);
  timer.restart();
  
  for (int currentSpeed = 0; currentSpeed <= MOTOR_SPEED; ) {
    setMotorsSpeed(currentSpeed);
    if (timer.isElapsed()) {
      currentSpeed++;
      timer.restart();
    }
  }
}

void setup() {
  pinMode(DIRECTION_PIN, INPUT_PULLUP);
  pinMode(STOP_PIN, INPUT_PULLUP);

  setMotorsSpeed(MOTOR_SPEED);
  runMotors(STOP);
}

void loop() {
  if (digitalRead(STOP_PIN) == HIGH) {
    directionState = STOP;
  } 
  else {
    directionState = digitalRead(DIRECTION_PIN) == HIGH ? AHEAD : BACK;
  }

  if (directionState != previousDirectionState) {
    if (directionState == STOP) {
      brake(ACCEL_DECEL_TIME);
    }
    else if (previousDirectionState == STOP) {
      accelerate(directionState, ACCEL_DECEL_TIME);
    }
    else {
      brake(ACCEL_DECEL_TIME);
      accelerate(directionState, ACCEL_DECEL_TIME);
    }
  }

  previousDirectionState = directionState;
}
