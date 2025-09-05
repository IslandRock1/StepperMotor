
#include <Arduino.h>
#include "stepper.h"

Stepper::Stepper(const StepperPinout &pinout)
    : enable_pins({pinout.enable0, pinout.enable1, pinout.enable2}),
      motor_pins({pinout.pin0, pinout.pin1, pinout.pin2, pinout.pin3}),
      encoders{Encoder(pinout.scl0, pinout.sdl0), Encoder(pinout.scl1, pinout.sdl1), Encoder(pinout.scl2, pinout.sdl2)}{

    for (const auto &p: motor_pins) {
        pinMode(p, OUTPUT);
        digitalWrite(p, LOW);
    }

    for (const auto &p: enable_pins) {
        pinMode(p, OUTPUT);
        digitalWrite(p, LOW);
    }
}

void Stepper::begin() {
    for (auto e : encoders) {
        e.begin();
    }

    updateAcceleration();
    digitalWrite(enable_pins.at(current_motor), HIGH);
}

void Stepper::step(const bool forward) {

    if (forward) {
        currentStep[current_motor]++;
        if (currentStep[current_motor] == 4) {
            currentStep[current_motor] = 0;
        }
    } else {
        currentStep[current_motor]--;
        if (currentStep[current_motor] == -1) {
            currentStep[current_motor] = 3;
        }
    }

    switch (currentStep[current_motor]) {
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

void Stepper::turnRotations(const int motor, const int rotations) {
    target_rotation[current_motor] += rotations;
    if (target_rotation[current_motor] < 0) {
        target_rotation[current_motor] += 4096;
    } else if (target_rotation[current_motor] > 4095) {
        target_rotation[current_motor] -= 4096;
    }

    last_step_time = micros();

    current_motor = motor;
    for (const auto &p : enable_pins) {
        digitalWrite(p, LOW);
    }
    digitalWrite(enable_pins.at(current_motor), HIGH);
}

void Stepper::update() {
    current_rotation[current_motor] = encoders.at(current_motor).readRotation();

    if (isFinished()) {
        // turn_off();
        return;
    }

    auto diff = target_rotation[current_motor] - current_rotation[current_motor];
    if (diff > 2048) {
        diff -= 4096;
    } else if (diff < -2048) {
        diff += 4096;
    }

    double delay_time;
    if (abs(diff) > acceleration_rotations) {
        delay_time = prev_delay[current_motor] - acceleration;
    } else {
        delay_time = prev_delay[current_motor] + acceleration;
    }

    delay_time = max(minimum_step_time, min(start_step_time, delay_time));
    prev_delay[current_motor] = delay_time;

    auto now = micros();
    if (now - last_step_time > delay_time) {
        Serial.print("Delay: ");
        Serial.print(delay_time);
        Serial.print(" | Remaining rots: ");
        Serial.print(abs(diff));
        Serial.print(" | Acc: ");
        Serial.println(acceleration);

        last_step_time = now;
        step(diff < 0);
    }
}

bool Stepper::isFinished() const {
    return abs(current_rotation[current_motor] - target_rotation[current_motor]) < 30;
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

