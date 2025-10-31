#include <AFMotor.h>

const int DIRECTION_PIN = 2;
const int STOP_PIN = A0;

const int MOTOR_SPEED = 100;
const int MOTOR_COUNT = 4;
const int ACCEL_DECEL_TIME = 1000;

const int STOP_WAIT_TIME = 3000;
const int LEFT_RIGHT_MOVE_TIME = 10000;

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

Timer stopWaitTimer(STOP_WAIT_TIME);
Timer leftRightMoveTimer(LEFT_RIGHT_MOVE_TIME);

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
        motors[i].run(i % 2 == 0 ? BACKWARD : FORWARD);
        break;
      case RIGHT:
        motors[i].run(i % 2 == 0 ? FORWARD : BACKWARD);
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
  pinMode(DIRECTION_PIN, INPUT);
  pinMode(STOP_PIN, INPUT);

  setMotorsSpeed(MOTOR_SPEED);
  runMotors(STOP);

  randomSeed(millis());
}

void loop() {
  if (randomDirectionState != STOP) {
    setMotorsSpeed(200);
    runMotors(randomDirectionState);

    if (leftRightMoveTimer.isElapsed()) {
      randomDirectionState = STOP;
    }
  }
  else {
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

    if (directionState == STOP && randomDirectionState == STOP) {
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
}
