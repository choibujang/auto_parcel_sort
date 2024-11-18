#include <Stepper.h>

const int STEP_N1 = 8;
const int STEP_N2 = 9;
const int STEP_N3 = 10;
const int STEP_N4 = 11;

Stepper kedexStepper(1024, STEP_N4, STEP_N2, STEP_N3, STEP_N1);

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    kedexStepper.setSpeed(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(millis());
  kedexStepper.step(256);
  kedexStepper.step(-256);
  kedexStepper.step(-256);
  Serial.println(millis());
  delay(1000);
}