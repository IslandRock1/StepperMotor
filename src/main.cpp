#include <Arduino.h>
#include <WiFi.h>

#include "stepper.h"
#include "timerStats.hpp"
#include "ESPNOW.hpp"

// 1A, 2A, 3A, 4A
Stepper stepper0{16, 17, 21, 18};
Stepper stepper1{22, 27, 14, 13};
Stepper stepper2{26, 25, 32, 33};

TimerStats timerStats;
ESPNOW espnow;

void OnDataRecv() {
    if (retrieving_data.NO_MOVE) {
        // Serial.println("NO_MOVE recieved.");
        // Happens CONSTANTLY (pinged by controller, to maintain com)
        return;
    }

    if (retrieving_data.version != 3) {
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

    byte data = retrieving_data.data;
    bool dir = (data >> 7) == 1;
    byte steps = data & 0b01111111;

    Serial.print("Dir: ");
    Serial.print(dir);
    Serial.print(" | Steps: ");
    Serial.println(steps);

    stepper0.setAccelerationDegrees(retrieving_data.acceleration_steps);
    stepper1.setAccelerationDegrees(retrieving_data.acceleration_steps);
    stepper2.setAccelerationDegrees(retrieving_data.acceleration_steps);

    stepper0.setStartStepTime(retrieving_data.start_time_div);
    stepper1.setStartStepTime(retrieving_data.start_time_div);
    stepper2.setStartStepTime(retrieving_data.start_time_div);

    stepper0.setMinStepTime(retrieving_data.min_time_div);
    stepper1.setMinStepTime(retrieving_data.min_time_div);
    stepper2.setMinStepTime(retrieving_data.min_time_div);

    switch (motorReceived) {
        case 1:
        {
            stepper0.turnSteps(steps, dir);
            stepper0.starting_time = millis();
        } break;

        case 2:
        {
            stepper1.turnSteps(steps, dir);
            stepper1.starting_time = millis();
        } break;

        case 3:
        {
            stepper2.turnSteps(steps, dir);
            stepper2.starting_time = millis();
        } break;

        default:
        {
            Serial.println("Somehow did not detect invalid motor earlier.");
        }
    }
}

void debugTest(bool dir) {
    byte steps = 100;

    Serial.print("Dir: ");
    Serial.print(dir);
    Serial.print(" | Steps: ");
    Serial.println(steps);

    retrieving_data.acceleration_steps = 20;
    retrieving_data.start_time_div = 250;
    retrieving_data.min_time_div = 250;

    stepper0.setAccelerationDegrees(retrieving_data.acceleration_steps);
    stepper1.setAccelerationDegrees(retrieving_data.acceleration_steps);
    stepper2.setAccelerationDegrees(retrieving_data.acceleration_steps);

    stepper0.setStartStepTime(retrieving_data.start_time_div * 40);
    stepper1.setStartStepTime(retrieving_data.start_time_div * 40);
    stepper2.setStartStepTime(retrieving_data.start_time_div * 40);

    stepper0.setMinStepTime(retrieving_data.min_time_div * 40);
    stepper1.setMinStepTime(retrieving_data.min_time_div * 40);
    stepper2.setMinStepTime(retrieving_data.min_time_div * 40);

    stepper0.turnSteps(steps, dir);
    stepper0.starting_time = millis();
}

uint8_t baseMac[6];
void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Serial configured.");

    espnow.begin();
}

unsigned long prevPrintTime = millis();
unsigned long prevSendDataTime = millis();
unsigned long prevDebugMove = millis();
bool debugDir = false;
void loop() {
    stepper0.update();
    stepper1.update();
    stepper2.update();
    auto is_finished = (stepper0.isFinished() and stepper1.isFinished() and stepper2.isFinished());

    if (is_finished and not stepper_data.is_finished) {
        // Raising edge, just finished.
        Serial.println("Finished movement in...");
        Serial.print("Stepper0 (ms): ");
        Serial.println(millis() - stepper0.starting_time);
        Serial.print("Stepper1 (ms): ");
        Serial.println(millis() - stepper1.starting_time);
        Serial.print("Stepper2 (ms): ");
        Serial.println(millis() - stepper2.starting_time);
        // Only the stepper that did the move will have a correct time..
        // Bad code, whatever.
    }
    stepper_data.is_finished = is_finished;

    if (ESPNOW::get_is_new_data()) {
        OnDataRecv();

        is_finished = (stepper0.isFinished() and stepper1.isFinished() and stepper2.isFinished());
        stepper_data.is_finished = is_finished;
        stepper_data.packet_id = retrieving_data.packet_id;
        stepper_data.motor_signature_id = my_id;

        espnow.send_data();
        ESPNOW::reset_is_new_data();
    }

    if (prevPrintTime + 500 < millis()) {
        prevPrintTime = millis();

        Serial.print("ID: ");
        Serial.print(my_id);
        Serial.print(" | Is finished: ");
        Serial.print(stepper_data.is_finished);
        Serial.print(" | Address: ");
        espnow.printAdr();
    }
}