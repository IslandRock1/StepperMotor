
#ifndef STEPPERMOTOR_STEPPER_H
#define STEPPERMOTOR_STEPPER_H

class Stepper {
public:
    Stepper(int pin0, int pin1, int pin2, int pin3, int enable = 0);
    void turnSteps(int num, bool dir);
    void update();
    bool isFinished() const;

    void setAccelerationSteps(int value);
    void setStartStepTime(int value);
    void setMinStepTime(int value);

private:
    int pin0;
    int pin1;
    int pin2;
    int pin3;
    int enablePin;

    int currentStep = 0;

    double start_step_time = 5000;
    double minimum_step_time = 3000;
    double acceleration_steps = 15;

    double acceleration = (start_step_time - minimum_step_time) / acceleration_steps;
    double current_acceleration_step = 0;

    double remaining_steps = 0;
    double finished_steps = 0;
    double current_time = start_step_time;
    bool direction = false;
    unsigned long last_step_time = 0;


    void step(bool forward);
};


#endif //STEPPERMOTOR_STEPPER_H
