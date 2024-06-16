#include <Arduino.h>
#include <array>

#include "stepper.h"

Stepper stepper{26, 27, 32, 33, 34};
std::array<bool, 5> activations;

enum State {
    OFF,
    Clockwise,
    Double,
    AntiClockwise
};

void push_back(bool e) {
    for (int i = 0; i < activations.size() - 1; i++) {
        activations[i] = activations[i + 1];
    }

    activations[activations.size() - 1] = e;
}

State compare() {

    int val = 0;
    for (auto &e : activations) {
        val *= 10;
        val += e;
    }

    switch (val) {
        case 11010:
        {
            return State::Double;
        } break;

        case 10100:
        {
            return State::Clockwise;
        }

        case 10011:
        {
            return State::AntiClockwise;
        } break;

        default:
        {
            return State::OFF;
        }
    }
}

void setup() {
    Serial.begin(9600);
    Serial.println();
    Serial.println("Serial configured.");

    pinMode(13, INPUT);
}

void loop() {
    if (millis() % 10 == 0) {
        push_back(digitalRead(13));
    } else {
        return;
    }

    auto s = compare();

    if (s != State::OFF) {
        activations = {false, false, false, false, false};
    }

    switch (s) {
        case State::Clockwise:
        {
            stepper.turnQuart(true);
        } break;

        case State::AntiClockwise:
        {
            stepper.turnQuart(false);
        }

        case State::Double:
        {
            stepper.turnHalf();
        }
    }
}