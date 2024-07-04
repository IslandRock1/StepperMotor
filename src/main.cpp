#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include "stepper.h"
#include "timerStats.hpp"

Stepper stepper{26, 27, 32, 33, 34};
TimerStats timerStats;

typedef struct StepperData {
    byte version;
    byte motorID;
    byte data;
} StepperData;

StepperData myData;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));

    if (myData.version != 0) {
        Serial.println("Wrong ESP_NOW version. main.cpp/OnDataRecv");
    }

    if (myData.motorID != 0) {
        Serial.println("Turning different motor.");
        return;
    }

    byte data = myData.data;
    bool dir = data >> 7;
    byte steps = data & 0b011111;

    Serial.print("ID: ");
    Serial.print(dir);
    Serial.print(" | Steps: ");
    Serial.println(steps);

    stepper.turnSteps(steps, dir);
}

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Serial configured.");

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error with ESP Now.");
    } else {
        esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
        Serial.println("ESP Now configured.");
    }
}

void loop() {

}