//
// Created by Øystein Bringsli.
//

#include <Arduino.h>
#include "stepper.h"

Stepper::Stepper(int pin0, int pin1, int pin2, int pin3, int enable)
    :pin0(pin0), pin1(pin1), pin2(pin2), pin3(pin3), enablePin(enable) {

    pinMode(pin0, OUTPUT);
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    pinMode(pin3, OUTPUT);
    pinMode(enablePin, OUTPUT);
    digitalWrite(enablePin, HIGH);
}

void Stepper::step(bool forward) {

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

void Stepper::turnQuart(bool forward) {
    for (int w = 0; w < 200; w++){
        step(forward);
        delay(pauseTime);
    }
}

void Stepper::turnHalf() {
    for (int w = 0; w < 400; w++) {
        step(true);
        delay(pauseTime);
    }
}

void Stepper::turnSteps(int num, bool dir) {
    for (int w = 0; w < num; w++) {
        step(dir);
        delay(pauseTime);
    }
}