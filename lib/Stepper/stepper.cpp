
#include <Arduino.h>
#include "stepper.hpp"

Stepper::Stepper(const StepperPinout &pinout)
    : _enable_pin(pinout.enable),
      _motor_pins({pinout.pin0, pinout.pin1, pinout.pin2, pinout.pin3}),
      _encoder{pinout.sdl, pinout.scl, pinout.BUS_NUM}{

    for (const auto &p: _motor_pins) {
        pinMode(p, OUTPUT);
        digitalWrite(p, LOW);
    }

    pinMode(_enable_pin, OUTPUT);
    digitalWrite(_enable_pin, HIGH);
}

void Stepper::begin() {
    updateAcceleration();
    _encoder.begin();
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
            digitalWrite(_motor_pins.at(0), HIGH);
            digitalWrite(_motor_pins.at(1), LOW);
            digitalWrite(_motor_pins.at(2), HIGH);
            digitalWrite(_motor_pins.at(3), LOW);
        } break;

        case 1:
        {
            digitalWrite(_motor_pins.at(0), LOW);
            digitalWrite(_motor_pins.at(1), HIGH);
            digitalWrite(_motor_pins.at(2), HIGH);
            digitalWrite(_motor_pins.at(3), LOW);
        } break;

        case 2:
        {
            digitalWrite(_motor_pins.at(0), LOW);
            digitalWrite(_motor_pins.at(1), HIGH);
            digitalWrite(_motor_pins.at(2), LOW);
            digitalWrite(_motor_pins.at(3), HIGH);
        } break;

        case 3:
        {
            digitalWrite(_motor_pins.at(0), HIGH);
            digitalWrite(_motor_pins.at(1), LOW);
            digitalWrite(_motor_pins.at(2), LOW);
            digitalWrite(_motor_pins.at(3), HIGH);
        } break;

        default:
        {
            throw std::runtime_error("Step count must be element in [0, 1, 2, 3].");
        }
    }
}

void Stepper::turn_off() const {
    digitalWrite(_motor_pins.at(0), LOW);
    digitalWrite(_motor_pins.at(1), LOW);
    digitalWrite(_motor_pins.at(2), LOW);
    digitalWrite(_motor_pins.at(3), LOW);
}

void Stepper::turnRotations(const int rotations) {
    Serial.print("Prev target: ");
    Serial.print(target_rotation);
    Serial.print(" | New: ");
    target_rotation += rotations;
    if (target_rotation < 0) {
        target_rotation += 4096;
    } else if (target_rotation > 4095) {
        target_rotation -= 4096;
    }
    Serial.print(target_rotation);

    last_step_time = micros();
    starting_time = last_step_time;
}

void Stepper::update() {
    int diff = 0;

    current_rotation = _encoder.getCumulativePosition();
    diff = target_rotation - current_rotation;

    if (diff > 2048) {
        diff -= 4096;
    } else if (diff < -2048) {
        diff += 4096;
    }

    publicDiff = diff;

    if (abs(diff) < innerHysterese) {
        if (active && (totTime == 0)) {
            totTime = micros() - starting_time;
        }
        active = false;
    }

    if (abs(diff) > outerHysterese) {
        active = true;
    }

    if (!active) {
        turn_off();
        return;
    }

    double delay_time;
    if (abs(diff) > acceleration_rotations) {
        delay_time = prev_delay - acceleration;
    } else {
        delay_time = prev_delay + acceleration;
    }

    delay_time = max(minimum_step_time, min(start_step_time, delay_time));
    prev_delay = delay_time;

    auto now = micros();
    if (now - last_step_time > delay_time) {
        Serial.print("Delay: ");
        Serial.print(delay_time);
        Serial.print(" | Remaining rots: ");
        Serial.print(abs(diff));
        Serial.print(" | Acc: ");
        Serial.print(acceleration);
        Serial.print(" | Dir: ");
        Serial.println(diff < 0);

        last_step_time = now;
        step(diff < 0);
    }
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

void Stepper::setHome() {
    _encoder.resetCumulativePosition();
}
