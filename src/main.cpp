#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

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

    if (myData.motorID != 5) {
        Serial.println("Turning different motor.");
        return;
    }

    byte data = myData.data;
    bool dir = (data >> 7) == 1;
    byte steps = data & 0b01111111;

    Serial.print("Dir: ");
    Serial.print(dir);
    Serial.print(" | Steps: ");
    Serial.println(steps);

    stepper.turnSteps(steps, dir);
}

uint8_t baseMac[6];
void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Serial configured.");

    WiFi.mode(WIFI_STA);
    esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error with ESP Now.");
    } else {
        esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
        Serial.println("ESP Now configured.");
    }
}

unsigned long prevPrintTime = millis();
void loop() {
    if (prevPrintTime + 500 < millis()) {
        prevPrintTime = millis();

        Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                      baseMac[0], baseMac[1], baseMac[2],
                      baseMac[3], baseMac[4], baseMac[5]);
    }
}