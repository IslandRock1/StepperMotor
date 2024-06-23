#include <Arduino.h>
#include <OneWire.h>
#include <array>

#include "stepper.h"
#include "timerStats.hpp"

OneWire oneWire(13);

Stepper stepper{26, 27, 32, 33, 34};
TimerStats timerStats;
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

State compare(byte data) {
    switch (data) {
        case 1:
        {
            return State::Clockwise;
        } break;

        case 2:
        {
            return State::Double;
        } break;

        case 3:
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
}

void loop() {

    byte data;
    if (oneWire.reset()) {
        data = oneWire.read();
        Serial.print("Reading data: ");
        Serial.println(data);
    } else {
        return;
    }

    auto s = compare(data);

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