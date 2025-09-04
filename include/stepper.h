
#ifndef STEPPERMOTOR_STEPPER_H
#define STEPPERMOTOR_STEPPER_H
#include <array>
#include <Wire.h>

struct Pinout {
    int pin0;
    int pin1;
    int pin2;
    int pin3;

    int enable0;
    int enable1;
    int enable2;
};

class Stepper {
public:
    explicit Stepper(const Pinout &pinout);
    void turnDegrees(int motor, int degrees, bool dir);
    void calibrate(bool dir);
    void update();
    bool isFinished() const;

    void setAccelerationDegrees(int value);
    void setStartStepTime(int value);
    void setMinStepTime(int value);
    void updateAcceleration();

    unsigned long starting_time = 0;

private:
    std::array<int, 3> enable_pins;
    std::array<int, 4> motor_pins;

    int currentStep = 0;
    int current_motor = 0;

    double start_step_time = 5000;
    double minimum_step_time = 3000;
    double acceleration_degrees = 15;

    double acceleration = (start_step_time - minimum_step_time) / acceleration_degrees;
    double current_acceleration_degrees = 0;

    int goal_degrees = 0;
    double remaining_degrees = 0;
    double current_time = start_step_time;
    bool direction = false;
    unsigned long last_step_time = 0;

    void step(bool forward);
};


#endif //STEPPERMOTOR_STEPPER_H
