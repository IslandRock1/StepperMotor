#include <Arduino.h>

#include "stepper.h"
#include "timerStats.hpp"


Stepper stepper{26, 27, 32, 33, 34};
TimerStats timerStats;

void setup() {
    Serial.begin(9600);
    Serial.println();
    Serial.println("Serial configured.");

    delay(5000);
}

void loop() {

    if (digitalRead(13)) {
        Serial.println("Turning.");
        stepper.turnQuart(true);
    }
}