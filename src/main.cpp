#include <Arduino.h>
#include <WiFi.h>

#include "stepper.h"
#include "timerStats.hpp"
#include "ESPNOW.hpp"

// 1A, 2A, 3A, 4A
StepperPinout motor_pin = {
    17, 16, 18, 21,
    14, 13, 22,
    25, 23, 26, 19, 27, 5
};

Stepper stepper{motor_pin};
TimerStats timerStats;
ESPNOW espnow;

void OnDataRecv() {
    if (retrieving_data.NO_MOVE) {
        // Serial.println("NO_MOVE recieved.");
        // Happens CONSTANTLY (pinged by controller, to maintain com)
        return;
    }

    if (retrieving_data.version != 5) {
        Serial.println("Wrong ESP_NOW version. main.cpp/OnDataRecv");
        return;
    }

    auto idRecieved = retrieving_data.motorID / 16;
    if (idRecieved != my_id) {
        Serial.println("Turning with different PCB.");
        return;
    }

    auto motorReceived = retrieving_data.motorID % 16;
    if ((motorReceived < 1) || (motorReceived > 3)) {
        Serial.println("Not a valid motor");
        return;
    }

    if (!stepper_data.is_finished) {
        Serial.println("Recieved data while not being ready.");
        return;
    }

    auto rots = stepper_data.rots;

    Serial.print("Rots: ");
    Serial.println(rots);

    stepper.setAccelerationDegrees(retrieving_data.acceleration_steps);
    stepper.setStartStepTime(retrieving_data.start_time_div);
    stepper.setMinStepTime(retrieving_data.min_time_div);

    stepper.turnRotations(motorReceived, rots);
    stepper.starting_time = millis();
}

uint8_t baseMac[6];
void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Serial configured.");

    espnow.begin();
    stepper.begin();
}

unsigned long prevPrintTime = millis();
int debugMoveIter = 0;
void loop() {
    stepper.update();
    auto is_finished = stepper.isFinished();
    stepper_data.is_finished = is_finished;

    if (ESPNOW::get_is_new_data()) {
        OnDataRecv();

        stepper_data.is_finished = is_finished;
        stepper_data.packet_id = retrieving_data.packet_id;
        stepper_data.motor_signature_id = my_id;

        espnow.send_data();
        ESPNOW::reset_is_new_data();
    }

    if (prevPrintTime + 100 < millis()) {
        prevPrintTime = millis();

        Serial.print("ID: ");
        Serial.print(my_id);
        Serial.print(" | Is finished: ");
        Serial.print(stepper_data.is_finished);
        Serial.print(" | Target rots: ");
        Serial.print(stepper.target_rotation[0]);
        Serial.print(" | Current rots: ");
        Serial.print(stepper.current_rotation[0]);
        Serial.print(" | Address: ");
        espnow.printAdr();

        debugMoveIter++;
        if (debugMoveIter == 100) {
            debugMoveIter = 0;
            // stepper.turnRotations(0, 16, false);
        }
    }
}