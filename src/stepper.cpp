//
// Created by Øystein Bringsli.
//

#include <Arduino.h>
#include "stepper.h"

#define maxPWM  255
#define threeFourthsPWM 180
#define halfPWM 128
#define minPWM    0

void setupPWM(int pin, int channel) {
    ledcSetup(channel, 10000, 8);
    ledcAttachPin(pin, channel);
}

Stepper::Stepper(int pin0, int pin1, int pin2, int pin3, int enable)
    :pin0(pin0), pin1(pin1), pin2(pin2), pin3(pin3), enablePin(enable) {

    setupPWM(pin0, 0);
    setupPWM(pin1, 1);
    setupPWM(pin2, 2);
    setupPWM(pin3, 3);

    pinMode(enablePin, OUTPUT);
    digitalWrite(enablePin, HIGH); // Was set to "OUTPUT" previously, might be error.
}

void Stepper::step(bool forward) {

    if (forward) {
        currentStep++;
        if (currentStep == 16) {
            currentStep = 0;
        }
    } else {
        currentStep--;
        if (currentStep == -1) {
            currentStep = 15;
        }
    }

    switch (currentStep) {
        case 0:
        {
            ledcWrite(0, maxPWM);
            ledcWrite(1, minPWM);
            ledcWrite(2, maxPWM);
            ledcWrite(3, minPWM);
        } break;

        case 1:
        {
            ledcWrite(0, threeFourthsPWM);
            ledcWrite(1, minPWM);
            ledcWrite(2, maxPWM);
            ledcWrite(3, minPWM);
        } break;

        case 2:
        {
            ledcWrite(0, minPWM);
            ledcWrite(1, minPWM);
            ledcWrite(2, maxPWM);
            ledcWrite(3, minPWM);
        } break;

        case 3:
        {
            ledcWrite(0, minPWM);
            ledcWrite(1, threeFourthsPWM);
            ledcWrite(2, maxPWM);
            ledcWrite(3, minPWM);
        } break;

        case 4:
        {
            ledcWrite(0, minPWM);
            ledcWrite(1, maxPWM);
            ledcWrite(2, maxPWM);
            ledcWrite(3, minPWM);
        } break;

        case 5:
        {
            ledcWrite(0, minPWM);
            ledcWrite(1, maxPWM);
            ledcWrite(2, threeFourthsPWM);
            ledcWrite(3, minPWM);
        } break;

        case 6:
        {
            ledcWrite(0, minPWM);
            ledcWrite(1, maxPWM);
            ledcWrite(2, minPWM);
            ledcWrite(3, minPWM);
        } break;

        case 7:
        {
            ledcWrite(0, minPWM);
            ledcWrite(1, maxPWM);
            ledcWrite(2, minPWM);
            ledcWrite(3, threeFourthsPWM);
        } break;

        case 8:
        {
            ledcWrite(0, minPWM);
            ledcWrite(1, maxPWM);
            ledcWrite(2, minPWM);
            ledcWrite(3, maxPWM);
        } break;

        case 9:
        {
            ledcWrite(0, minPWM);
            ledcWrite(1, threeFourthsPWM);
            ledcWrite(2, minPWM);
            ledcWrite(3, maxPWM);
        } break;

        case 10:
        {
            ledcWrite(0, minPWM);
            ledcWrite(1, minPWM);
            ledcWrite(2, minPWM);
            ledcWrite(3, maxPWM);
        } break;

        case 11:
        {
            ledcWrite(0, threeFourthsPWM);
            ledcWrite(1, minPWM);
            ledcWrite(2, minPWM);
            ledcWrite(3, maxPWM);
        } break;

        case 12:
        {
            ledcWrite(0, maxPWM);
            ledcWrite(1, minPWM);
            ledcWrite(2, minPWM);
            ledcWrite(3, maxPWM);
        } break;

        case 13:
        {
            ledcWrite(0, maxPWM);
            ledcWrite(1, minPWM);
            ledcWrite(2, minPWM);
            ledcWrite(3, threeFourthsPWM);
        } break;

        case 14:
        {
            ledcWrite(0, maxPWM);
            ledcWrite(1, minPWM);
            ledcWrite(2, minPWM);
            ledcWrite(3, minPWM);
        } break;

        case 15:
        {
            ledcWrite(0, maxPWM);
            ledcWrite(1, minPWM);
            ledcWrite(2, threeFourthsPWM);
            ledcWrite(3, minPWM);
        } break;

        default:
        {
            throw std::runtime_error("Step count must be element in range 0 to 15.");
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