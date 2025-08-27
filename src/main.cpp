#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "stepper.h"
#include "timerStats.hpp"

// 1A, 2A, 3A, 4A
Stepper stepper0{16, 17, 21, 18};
Stepper stepper1{22, 27, 14, 13};
Stepper stepper2{26, 25, 32, 33};

TimerStats timerStats;
int myID = 1;

typedef struct StepperData {
    byte version;
    byte motorID;
    byte data;
    byte acceleration_steps;
    byte start_time_div20;
    byte min_time_div20;
} StepperData;

StepperData myData;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));

    if (myData.version != 2) {
        Serial.println("Wrong ESP_NOW version. main.cpp/OnDataRecv");
    }

    auto idRecieved = myData.motorID / 16;
    if (idRecieved != myID) {
        Serial.println("Turning with different PCB");
        return;
    }

    auto motorReceived = myData.motorID % 16;
    if ((motorReceived < 1) || (motorReceived > 3)) {
        Serial.println("Not a valid motor");
        return;
    }

    byte data = myData.data;
    bool dir = (data >> 7) == 1;
    byte steps = data & 0b01111111;

    Serial.print("Dir: ");
    Serial.print(dir);
    Serial.print(" | Steps: ");
    Serial.println(steps);

    stepper0.setAccelerationSteps(myData.acceleration_steps);
    stepper1.setAccelerationSteps(myData.acceleration_steps);
    stepper2.setAccelerationSteps(myData.acceleration_steps);

    stepper0.setStartStepTime(myData.start_time_div20 * 20);
    stepper1.setStartStepTime(myData.start_time_div20 * 20);
    stepper2.setStartStepTime(myData.start_time_div20 * 20);

    stepper0.setMinStepTime(myData.min_time_div20 * 20);
    stepper1.setMinStepTime(myData.min_time_div20 * 20);
    stepper2.setMinStepTime(myData.min_time_div20 * 20);

    switch (motorReceived) {
        case 1:
        {
            stepper0.turnSteps(steps, dir);
        } break;

        case 2:
        {
            stepper1.turnSteps(steps, dir);
        } break;

        case 3:
        {
            stepper2.turnSteps(steps, dir);
        } break;

        default:
        {
            Serial.println("Somehow did not detect invalid motor earlier.");
        }
    }
}

uint8_t baseMac[6];
void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Serial configured.");

    WiFi.mode(WIFI_STA);
    esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
    if (ret != ESP_OK) {
        Serial.println("Failed to get MAC adress.");
    }

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error with ESP Now.");
    } else {
        esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
        Serial.println("ESP Now configured.");
    }
}

unsigned long prevPrintTime = millis();
void loop() {
    stepper0.update();
    stepper1.update();
    stepper2.update();

    if (prevPrintTime + 500 < millis()) {
        prevPrintTime = millis();

        Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                      baseMac[0], baseMac[1], baseMac[2],
                      baseMac[3], baseMac[4], baseMac[5]);

        Serial.print("ID: ");
        Serial.println(myID);
    }
}