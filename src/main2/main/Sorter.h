#include <Stepper.h>

class Sorter {
    int stepN1Pin;
    int stepN2Pin;
    int stepN3Pin;
    int stepN4Pin;
    int detectPin;

    int sortStatus;
    Stepper stepper;
public:
    Sorter(int p1, int p2, int p3, int p4, int d): 
        stepN1Pin(p1), stepN2Pin(p2), stepN3Pin(p3), stepN4Pin(p4), detectPin(d),
        stepper(1024, stepN4Pin, stepN2Pin, stepN3Pin, stepN1Pin)
    {}


};