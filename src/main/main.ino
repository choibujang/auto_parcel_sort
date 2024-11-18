#include "KedexController.h"

const int STEP_N1 = 8;
const int STEP_N2 = 9;
const int STEP_N3 = 10;
const int STEP_N4 = 11;

const int DETECT_PIN = 13;

const int ENB = 6;
const int IN3 = 4;
const int IN4 = 5;



KedexController kedex(STEP_N1,STEP_N2,STEP_N3,STEP_N4,DETECT_PIN,ENB,IN3,IN4);

void setup() {
    Serial.begin(9600);
    pinMode(DETECT_PIN, INPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}
/* 0: waiting
   1: get signal
   2: moving
   3: arrivied
*/

void loop() {
    kedex.serialRead();

    if (kedex.getKedexStatus() == 0) {
        if (!kedex.product_queue.isEmpty()) {
            kedex.setKedexStatus(1);
        }
    } else if (kedex.getKedexStatus() == 1) {
        if(kedex.detect()) {
            kedex.setStepperToPos();
            kedex.waitingStartTime = millis();
            kedex.setKedexStatus(2);
        }
    } else if (kedex.getKedexStatus() == 2) {
        if (millis - kedex.waitingStartTime > 1000) {
            kedex.kedexStepper.step(-1 * kedex.stepperPos);
            kedex.setKedexStatus(3);
        }

    } else if (kedex.getKedexStatus() == 3) {
        if (kedex.product_queue.isEmpty()) {
            kedex.setKedexStatus(0);
        } else {
            kedex.setKedexStatus(1);
        }
    }

    kedex.conveyorMove();

}
