
#ifndef STEPPERMOTOR_STEPPER_H
#define STEPPERMOTOR_STEPPER_H

#include <array>
#include "Encoder.hpp"

struct StepperPinout {
    int pin0;
    int pin1;
    int pin2;
    int pin3;
    int enable;
    int sdl, scl, BUS_NUM;
};

class Stepper {
public:
    explicit Stepper(const StepperPinout &pinout);
    void begin();
    void update();

    void turnRotations(int rotations);

    void setAccelerationDegrees(int value);
    void setStartStepTime(int value);
    void setMinStepTime(int value);
    void updateAcceleration();

    void setHome();

    unsigned long starting_time = 0;
    unsigned long totTime = 0;
    signed int publicDiff = 0;
    bool active = false;

    // Start time | Min time | Accel rots | Tot time
    //   50000.00 | 40000.00 |     500.00 |  2429942
    //   40000.00 | 30000.00 |     500.00 |  1878888
    //   30000.00 | 20000.00 |     500.00 |  1332846
    //   20000.00 | 10000.00 |     500.00 |   775964 (Sub 1 second)
    //   10000.00 |  8000.00 |     500.00 |   497478 (0.5 Seconds)
    //   10000.00 |  8000.00 |     300.00 |   470327
    //    9000.00 |  8000.00 |     300.00 |   437500
    //    8000.00 |  8000.00 |     300.00 |   421801
    //    7000.00 |  7000.00 |     300.00 |   397247
    //    6000.00 |  6000.00 |     300.00 |   328214 (Sub 1/3 second)
    //    5000.00 |  5000.00 |     300.00 |   296440
    //    5000.00 |  4000.00 |     300.00 |   291728
    //    5000.00 |  4000.00 |     500.00 |   289883 (ish 200 rpm)

    double start_step_time        = 5000;
    double minimum_step_time      = 4000;
    double acceleration_rotations =  500;

private:
    int _enable_pin;
    std::array<int, 4> _motor_pins;

    SensorAS5600 _encoder;

    int currentStep = 0;

    int outerHysterese = 50;
    int innerHysterese =  3;
    double acceleration = 0;

    //target and current here
    signed int target_rotation = 0;
    signed int current_rotation = 0;
    double prev_delay = 10000;
    unsigned long last_encoder_read = 0;

    bool direction = false;
    unsigned long last_step_time = 0;

    void step(bool forward);
    void turn_off() const;
};


#endif //STEPPERMOTOR_STEPPER_H
