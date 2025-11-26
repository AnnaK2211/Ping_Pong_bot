#include <AFMotor.h>

const int DIRECTION_PIN = 2;
const int STOP_PIN = A0;

const int MOTOR_SPEED = 100;
const int MOTOR_TURNING_SPEED = 190;
const int MOTOR_COUNT = 4;
const unsigned long ACCEL_DECEL_TIME = 700;
const unsigned long STOP_WAIT_TIME = 3000;
const unsigned long TURN_TIME = 800;

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

    void stop() {
      timerStart = 0;
    }

    bool isElapsed() {
      return isRunning() && millis() - timerStart >= duration;
    }

    bool isRunning() {
      return timerStart != 0;
    }
};

AF_DCMotor motors[MOTOR_COUNT] = {AF_DCMotor(1), AF_DCMotor(2), AF_DCMotor(3), AF_DCMotor(4)};

enum Direction {
  AHEAD,
  BACK,
  LEFT,
  RIGHT,
  STOP
};

Direction directionState = STOP;
Direction previousDirectionState = STOP;
Direction randomDirectionState = STOP;

Timer stopWaitTimer(STOP_WAIT_TIME);
Timer leftRightMoveTimer(TURN_TIME);

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
      case LEFT:
        motors[i].run((i < (MOTOR_COUNT / 2)) ? BACKWARD : FORWARD);
        break;
      case RIGHT:
        motors[i].run((i < (MOTOR_COUNT / 2)) ? FORWARD : BACKWARD);
        break;
    }
  }
}

void brake(unsigned long duration) {
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

void accelerate(Direction newDirection, unsigned long duration) {
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

void doRandomTurn() {
  if (randomDirectionState != STOP) {
    setMotorsSpeed(MOTOR_TURNING_SPEED);
    runMotors(randomDirectionState);

    if (leftRightMoveTimer.isElapsed()) {
      leftRightMoveTimer.stop();
      randomDirectionState = STOP;
    }
  }
  else if (directionState == STOP) {
    if (!stopWaitTimer.isRunning()) {
      stopWaitTimer.restart();
    }

    if (stopWaitTimer.isElapsed()) {
      stopWaitTimer.stop();
      randomDirectionState = random(LEFT, STOP);
      leftRightMoveTimer.restart();
    }
  }
}

void updatePingPongMotion() {
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

void setup() {
  pinMode(DIRECTION_PIN, INPUT);
  pinMode(STOP_PIN, INPUT);

  setMotorsSpeed(MOTOR_SPEED);
  runMotors(STOP);

  randomSeed(millis());
}

void loop() {
  if (randomDirectionState != STOP) {
    doRandomTurn();
  }
  else {
    updatePingPongMotion();
    doRandomTurn();
  }
}
