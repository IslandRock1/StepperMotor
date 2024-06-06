//
// Created by Øystein Bringsli.
//

#ifndef STEPPERMOTOR_STEPPER_H
#define STEPPERMOTOR_STEPPER_H


class Stepper {
public:
    Stepper(int pin0, int pin1, int pin2, int pin3, int enable);
    void turnQuart(bool forward);
    void turnHalf();
    void step(bool forward);

private:
    int pin0;
    int pin1;
    int pin2;
    int pin3;
    int enablePin;

    int currentStep = 0;
    int pauseTime = 10;
};


#endif //STEPPERMOTOR_STEPPER_H
