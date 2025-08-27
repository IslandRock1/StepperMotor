
#include <Arduino.h>
#include "stepper.h"

Stepper::Stepper(int pin0, int pin1, int pin2, int pin3, int enable)
    :pin0(pin0), pin1(pin1), pin2(pin2), pin3(pin3), enablePin(enable) {

    pinMode(pin0, OUTPUT);
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    pinMode(pin3, OUTPUT);

    if (enablePin != 0) {
        pinMode(enablePin, OUTPUT);
        digitalWrite(enablePin, HIGH);
    }
}

void Stepper::step(const bool forward) {

    if (forward) {
        currentStep++;
        if (currentStep == 4) {
            currentStep = 0;
        }
    } else {
        currentStep--;
        if (currentStep == -1) {
            currentStep = 3;
        }
    }

    switch (currentStep) {
        case 0:
        {
            digitalWrite(pin0, HIGH);
            digitalWrite(pin1, LOW);
            digitalWrite(pin2, HIGH);
            digitalWrite(pin3, LOW);
        } break;

        case 1:
        {
            digitalWrite(pin0, LOW);
            digitalWrite(pin1, HIGH);
            digitalWrite(pin2, HIGH);
            digitalWrite(pin3, LOW);
        } break;

        case 2:
        {
            digitalWrite(pin0, LOW);
            digitalWrite(pin1, HIGH);
            digitalWrite(pin2, LOW);
            digitalWrite(pin3, HIGH);
        } break;

        case 3:
        {
            digitalWrite(pin0, HIGH);
            digitalWrite(pin1, LOW);
            digitalWrite(pin2, LOW);
            digitalWrite(pin3, HIGH);
        } break;

        default:
        {
            throw std::runtime_error("Step count must be element in [0, 1, 2, 3].");
        }
    }
}

void Stepper::turnSteps(const int num, const bool dir) {
    remaining_steps = num;
    finished_steps = 0;
    direction = dir;
    current_acceleration_step = 0;
    last_step_time = micros();
}

void Stepper::update() {
    if (remaining_steps == 0) {return;}

    double delay_time;
    if (finished_steps < acceleration_steps) {
        delay_time = start_step_time - acceleration * finished_steps;
    } else if (remaining_steps < acceleration_steps) {
        delay_time = start_step_time - acceleration * remaining_steps;
    } else {
        delay_time = minimum_step_time;
    }

    if (micros() - last_step_time > delay_time) {
        step(direction);
        last_step_time = micros();

        finished_steps++;
        remaining_steps--;
    }
}

bool Stepper::isFinished() const {
    return (remaining_steps == 0);
}

void Stepper::setAccelerationSteps(int value) {
    acceleration_steps = value;
}

void Stepper::setStartStepTime(int value) {
    start_step_time = value;
}

void Stepper::setMinStepTime(int value) {
    minimum_step_time = value;
}

