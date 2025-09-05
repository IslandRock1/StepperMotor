
#include <Arduino.h>
#include "stepper.h"

Stepper::Stepper(const Pinout &pinout)
    : enable_pins({pinout.enable0, pinout.enable1, pinout.enable2}),
      motor_pins({pinout.pin0, pinout.pin1, pinout.pin2, pinout.pin3}),
      encoders{Encoder(pinout.scl0, pinout.sda0), Encoder(pinout.scl1, pinout.sda1), Encoder(pinout.scl2, pinout.sda2)}{

    for (const auto &p: motor_pins) {
        pinMode(p, OUTPUT);
        digitalWrite(p, LOW);
    }

    for (const auto &p: enable_pins) {
        pinMode(p, OUTPUT);
        digitalWrite(p, LOW);
    }
}

void Stepper::begin() const {
    for (auto e : encoders) {
        e.begin();
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

void Stepper::turn_off() const {
    digitalWrite(motor_pins.at(0), LOW);
    digitalWrite(motor_pins.at(1), LOW);
    digitalWrite(motor_pins.at(2), LOW);
    digitalWrite(motor_pins.at(3), LOW);
}

void Stepper::turnRotations(const int motor, const int rotations, const bool dir) {
    target_rotation += rotations * (dir * 2 - 1);
    direction = dir;
    last_step_time = micros();

    current_motor = motor;
    for (const auto &p : enable_pins) {
        digitalWrite(p, LOW);
    }
    digitalWrite(enable_pins.at(current_motor), HIGH);
}

void Stepper::update() {
    current_rotation = encoders.at(current_motor).readRotation();

    if (isFinished()) {
        turn_off();
        return;
    }

    // Cases

    auto one_way = target_rotation - current_rotation;
    auto other_way  = current_rotation - (target_rotation - 4096);
    if (one_way < other_way) {
        // Step down
    } else {
        // Step up
    }

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
    return abs(current_rotation - target_rotation) < 5;
}

void Stepper::setAccelerationDegrees(int value) {
    acceleration_rotations = value;
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
    acceleration = (start_step_time - minimum_step_time) / acceleration_rotations;
}

