
#include <Arduino.h>
#include "stepper.hpp"

#include "ESPNOW.hpp"

Stepper::Stepper(const StepperPinout &pinout)
    : enable_pins({pinout.enable0, pinout.enable1, pinout.enable2}),
      motor_pins({pinout.pin0, pinout.pin1, pinout.pin2, pinout.pin3}),
      encoder{pinout.scl, pinout.sdl, pinout.A, pinout.B}{

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
    updateAcceleration();
    digitalWrite(enable_pins.at(current_motor), HIGH);

    encoder.begin();

    if (my_id == 1) {
        target_rotation[0] = 200;
        target_rotation[1] = 2450;
        target_rotation[2] = 2950;
    } else {
        target_rotation[0] = 3900;
        target_rotation[1] = 560;
        target_rotation[2] = 950;
    }
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
    Serial.print("Prev target: ");
    Serial.print(target_rotation[motor]);
    Serial.print(" | New: ");
    target_rotation[motor] += rotations;
    // if (target_rotation[motor] < 0) {
    //     target_rotation[motor] += 4096;
    // } else if (target_rotation[motor] > 4095) {
    //     target_rotation[motor] -= 4096;
    // }
    Serial.print(target_rotation[motor]);
    Serial.print(" | Motor: ");
    Serial.println(motor);

    last_step_time = micros();
}

void Stepper::update() {
    int diff[3] = {};

    for (int i = 0; i < 3; i++) {
        current_rotation[i] = encoder.getCumPosition(i);

        diff[i] = target_rotation[i] - current_rotation[i];

        if (diff[i] > 2048) {
            diff[i] -= 4096;
        } else if (diff[i] < -2048) {
            diff[i] += 4096;
        }

        publicDiff[i] = diff[i];

        if (abs(diff[i]) > outerHysterese) {
            active[i] = true;
        }

        if (abs(diff[i]) < innerHysterese) {
            active[i] = false;
        }
    }

    if (isFinished()) {
        turn_off();
        return;
    }

    bool directionLoc;
    if ((abs(diff[0]) > abs(diff[1])) && (abs(diff[0]) > abs(diff[2])) && active[0]) {
        current_motor = 0;
    } else if (abs(diff[1]) > abs(diff[2]) && active[1]) {
        current_motor = 1;
    } else if (active[2]) {
        current_motor = 2;
    } else {
        turn_off();
        return;
    }

    if (my_id == 1) {
        if (current_motor == 0) {
            directionLoc = diff[current_motor] < 0;
        } else if (current_motor == 1) {
            directionLoc = diff[current_motor] < 0;
        } else {
            directionLoc = diff[current_motor] < 0;
        }
    } else {
        if (current_motor == 0) {
            directionLoc = diff[current_motor] < 0;
        } else if (current_motor == 1) {
            directionLoc = diff[current_motor] < 0;
        } else {
            directionLoc = diff[current_motor] < 0;
        }
    }

    return;
    for (const auto &p : enable_pins) {
        digitalWrite(p, LOW);
    }
    digitalWrite(enable_pins.at(current_motor), HIGH);

    double delay_time;
    if (abs(diff[current_motor]) > acceleration_rotations) {
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
        Serial.print(abs(diff[current_motor]));
        Serial.print(" | Acc: ");
        Serial.print(acceleration);
        Serial.print(" | Dir: ");
        Serial.println(directionLoc);

        last_step_time = now;
        step(directionLoc);
    }
}

bool Stepper::isFinished() const {
    for (bool i : active) {
        if (i) {
            return false;
        }
    }

    return true;
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

void Stepper::setPrecisionRots(int value) {
    precision_rots = value;
}


void Stepper::updateAcceleration() {
    acceleration = (start_step_time - minimum_step_time) / acceleration_rotations;
}

void Stepper::setHome() {
    encoder.setCumPosition();
    target_rotation[0] = 0;
    target_rotation[1] = 0;
    target_rotation[2] = 0;
}

