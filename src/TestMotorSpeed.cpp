#include <Arduino.h>
#include "stepper.hpp"

// 1A, 2A, 3A, 4A
StepperPinout motor_pin = {
    17, 16, 18, 21,
    14,
    27, 5, 0
};

Stepper stepper{motor_pin};

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Serial configured.");

    pinMode(13, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(22, OUTPUT);
    digitalWrite(14, LOW);
    digitalWrite(13, HIGH);
    digitalWrite(22, LOW);

    // Encoder select
    pinMode(23, OUTPUT);
    pinMode(25, OUTPUT);
    digitalWrite(23, LOW);
    digitalWrite(25, LOW);

    stepper.begin();
    stepper.setHome();
}

int debugIter = 1000000000;
unsigned long prevPrintTime = millis();
void loop() {
    stepper.update();

    unsigned int pauseTime = 100;
    if (prevPrintTime + pauseTime < millis()) {
        prevPrintTime = millis();

        Serial.print(" | Is active: ");
        Serial.print(stepper.active);
        Serial.print(" | Diff0: ");
        Serial.print(stepper.publicDiff);
        Serial.print(" | Debug iter: ");
        Serial.print(debugIter);
        Serial.print(" | ");
        Serial.print(stepper.start_step_time);
        Serial.print(" | ");
        Serial.print(stepper.minimum_step_time);
        Serial.print(" | ");
        Serial.print(stepper.acceleration_rotations);
        Serial.print(" | ");
        Serial.println(stepper.totTime);

        debugIter++;
        auto wantedTime = 5 * 1000;
        if (debugIter > (wantedTime / pauseTime)) {
            debugIter = 0;
            stepper.turnRotations(1024);
            stepper.totTime = 0;
        }
    }
}