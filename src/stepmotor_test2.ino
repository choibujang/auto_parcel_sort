#include <Stepper.h>

const int STEP_N1 = 8;
const int STEP_N2 = 9;
const int STEP_N3 = 10;
const int STEP_N4 = 11;

Stepper stepper(2048, STEP_N4, STEP_N2, STEP_N3, STEP_N1);
int currStepperPos = 0;
int absStepperPos = 0;

void moveToPosition(int absStepperPos) {
    Serial.println("moveToPosition entered");
    stepper.step(absStepperPos - currStepperPos);
    currStepperPos = absStepperPos;
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    stepper.setSpeed(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  moveToPosition(256);
  delay(3000);
  moveToPosition(-256);

  delay(3000);
}