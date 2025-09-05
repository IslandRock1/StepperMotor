
#ifndef STEPPERMOTOR_STEPPER_H
#define STEPPERMOTOR_STEPPER_H
#include <array>

#include "Encoder.hpp"

struct Pinout {
    int pin0;
    int pin1;
    int pin2;
    int pin3;

    int enable0;
    int enable1;
    int enable2;

    int sda0, scl0;
    int sda1, scl1;
    int sda2, scl2;
};

class Stepper {
public:
    explicit Stepper(const Pinout &pinout);
    void begin() const;
    void update();

    void turnRotations(int motor, int rotations, bool dir);
    void calibrate(bool dir);
    bool isFinished() const;

    void setAccelerationDegrees(int value);
    void setStartStepTime(int value);
    void setMinStepTime(int value);
    void updateAcceleration();

    unsigned long starting_time = 0;

private:
    std::array<int, 3> enable_pins;
    std::array<int, 4> motor_pins;

    std::array<Encoder, 3> encoders;

    int currentStep = 0;
    int current_motor = 0;

    double start_step_time = 5000;
    double minimum_step_time = 3000;
    double acceleration_rotations = 500;
    double acceleration = 0;

    signed int target_rotation = 0;
    signed int current_rotation = 0;

    bool direction = false;
    unsigned long last_step_time = 0;

    void step(bool forward);
    void turn_off() const;
};


#endif //STEPPERMOTOR_STEPPER_H
