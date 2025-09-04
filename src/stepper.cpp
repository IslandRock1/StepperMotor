
#include <Arduino.h>
#include "stepper.h"

Stepper::Stepper(const Pinout &pinout)
    : enable_pins({pinout.enable0, pinout.enable1, pinout.enable2}),
    motor_pins({pinout.pin0, pinout.pin1, pinout.pin2, pinout.pin3}){

    for (const auto &p : motor_pins) {
        pinMode(p, OUTPUT);
        digitalWrite(p, LOW);
    }

    for (const auto &p : enable_pins) {
        pinMode(p, OUTPUT);
        digitalWrite(p, LOW);
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
            digitalWrite(motor_pins.at(0), HIGH);
            digitalWrite(motor_pins.at(1), LOW);
            digitalWrite(motor_pins.at(2), HIGH);
            digitalWrite(motor_pins.at(3), LOW);
        } break;

        case 1:
        {
            digitalWrite(motor_pins.at(0), LOW);
            digitalWrite(motor_pins.at(1), HIGH);
            digitalWrite(motor_pins.at(2), HIGH);
            digitalWrite(motor_pins.at(3), LOW);
        } break;

        case 2:
        {
            digitalWrite(motor_pins.at(0), LOW);
            digitalWrite(motor_pins.at(1), HIGH);
            digitalWrite(motor_pins.at(2), LOW);
            digitalWrite(motor_pins.at(3), HIGH);
        } break;

        case 3:
        {
            digitalWrite(motor_pins.at(0), HIGH);
            digitalWrite(motor_pins.at(1), LOW);
            digitalWrite(motor_pins.at(2), LOW);
            digitalWrite(motor_pins.at(3), HIGH);
        } break;

        default:
        {
            throw std::runtime_error("Step count must be element in [0, 1, 2, 3].");
        }
    }
}

void Stepper::turnDegrees(const int motor, const int degrees, const bool dir) {
    remaining_degrees = degrees;
    finished_degrees = 0;
    direction = dir;
    current_acceleration_degrees = 0;
    last_step_time = micros();

    current_motor = motor;
    for (const auto &p : enable_pins) {
        digitalWrite(p, LOW);
    }
    digitalWrite(enable_pins.at(current_motor), HIGH);
}

void Stepper::update() {
    if (remaining_degrees == 0) {return;}

    double delay_time;
    if (finished_degrees < acceleration_degrees) {
        delay_time = start_step_time - acceleration * finished_degrees;
    } else if (remaining_degrees < acceleration_degrees) {
        delay_time = start_step_time - acceleration * remaining_degrees;
    } else {
        delay_time = minimum_step_time;
    }

    auto now = micros();
    if (now - last_step_time > delay_time) {
        Serial.print("Delay: ");
        Serial.print(delay_time);
        Serial.print(" | Finished steps: ");
        Serial.print(finished_degrees);
        Serial.print(" | Remaining steps: ");
        Serial.println(remaining_degrees);

        last_step_time = now;
        finished_degrees++;
        remaining_degrees--;

        step(direction);
    }
}

bool Stepper::isFinished() const {
    return (remaining_degrees == 0);
}

void Stepper::setAccelerationDegrees(int value) {
    acceleration_degrees = value;
    updateAcceleration();
}

void Stepper::setStartStepTime(int value) {
    start_step_time = value;
    updateAcceleration();
}

void Stepper::setMinStepTime(int value) {
    minimum_step_time = value;
    updateAcceleration();
}

void Stepper::updateAcceleration() {
    acceleration = (start_step_time - minimum_step_time) / acceleration_degrees;
}

