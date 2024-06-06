#include <Arduino.h>
#include "stepper.h"

Stepper stepper{26, 27, 32, 33, 34};
void setup() {
    Serial.begin(9600);
    Serial.println();
    Serial.println("Serial configured.");

    pinMode(13, INPUT);
}

void loop() {
    if (digitalRead(13)) {
        Serial.println("Turning!");
        stepper.turnHalf();
    }
}