
#ifndef STEPPERMOTOR_STEPPER_H
#define STEPPERMOTOR_STEPPER_H


class Stepper {
public:
    Stepper(int pin0, int pin1, int pin2, int pin3, int enable = 0);
    void turnQuart(bool forward);
    void turnHalf();
    void turnSteps(int num, bool dir);

private:
    int pin0;
    int pin1;
    int pin2;
    int pin3;
    int enablePin;

    int currentStep = 0;
    int pauseTime = 10;

    void step(bool forward);
};


#endif //STEPPERMOTOR_STEPPER_H
