
#ifndef STEPPERMOTOR_STEPPER_H
#define STEPPERMOTOR_STEPPER_H

#include <array>
#include "Encoder.hpp"

struct StepperPinout {
    int pin0;
    int pin1;
    int pin2;
    int pin3;

    int enable0;
    int enable1;
    int enable2;

    int sdl, scl;
    int A, B;
};

class Stepper {
public:
    explicit Stepper(const StepperPinout &pinout);
    void begin();
    void update();

    void turnRotations(int motor, int rotations);
    bool isFinished() const;

    void setAccelerationDegrees(int value);
    void setStartStepTime(int value);
    void setMinStepTime(int value);
    void setPrecisionRots(int value);
    void updateAcceleration();

    unsigned long starting_time = 0;

    // Move to private
    signed int target_rotation[3] = {};
    signed int current_rotation[3] = {};
    signed int publicDiff[3] = {};
    int current_motor = 0;

private:
    std::array<int, 3> enable_pins;
    std::array<int, 4> motor_pins;

    Encoder encoder;

    int currentStep[3] = {0, 0, 0};
    //int current_motor = 0;

    double start_step_time = 50000;
    double minimum_step_time = 40000;
    double acceleration_rotations = 500;
    int precision_rots = 50;
    double acceleration = 0;

    //target and current here
    double prev_delay[3] = {10000, 10000, 10000};
    unsigned long last_encoder_read = 0;

    bool direction[3] = {false, false, false};
    unsigned long last_step_time = 0;

    void step(bool forward);
    void turn_off() const;
};


#endif //STEPPERMOTOR_STEPPER_H
