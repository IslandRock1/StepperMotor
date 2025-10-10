#include "Encoder.hpp"
#include <Wire.h>

#include "ESPNOW.hpp"

Encoder::Encoder(const int scl, const int sda, const int A, const int B)
    : scl_pin(scl), sda_pin(sda), pinA(A), pinB(B) {
}

void Encoder::begin() {
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);

    // Initialize I2C only once
    Wire.begin(sda_pin, scl_pin);
    Wire.setClock(100000); // 100kHz standard speed

    Serial.print("Clock pin: ");
    Serial.println(scl_pin);

    // Allow time for devices to power up
    // delay(10);
}

void Encoder::switchEncoder(int motor) {
    if (my_id == 1) {
        switch(motor) {
            case 1:
                digitalWrite(pinA, LOW);
                digitalWrite(pinB, LOW);
                break;
            case 2:
                digitalWrite(pinA, LOW);
                digitalWrite(pinB, HIGH);
                break;
            case 0:
                digitalWrite(pinA, HIGH);
                digitalWrite(pinB, LOW);
                break;
            default:
                Serial.println("Invalid encoder selected.");
                return;
        }
    } else {
        switch(motor) {
            case 2:
                digitalWrite(pinA, LOW);
                digitalWrite(pinB, LOW);
                break;
            case 1:
                digitalWrite(pinA, LOW);
                digitalWrite(pinB, HIGH);
                break;
            case 0:
                digitalWrite(pinA, HIGH);
                digitalWrite(pinB, LOW);
                break;
            default:
                Serial.println("Invalid encoder selected.");
                return;
        }
    }
    delayMicroseconds(100); // Increased settling time
}

bool Encoder::readRawAngle(uint16_t &angle) {
    Wire.beginTransmission(ADDRESS);
    Wire.write(RAW_ANGLE_MSB);
    uint8_t error = Wire.endTransmission(false); // Repeated start

    if (error != 0) {
        Serial.print("I2C transmission error: ");
        Serial.println(error);
        return false;
    }

    uint8_t bytesRead = Wire.requestFrom(ADDRESS, (uint8_t)2);
    if (bytesRead != 2) {
        Serial.print("Incomplete data received. Expected 2, got ");
        Serial.println(bytesRead);
        return false;
    }

    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();

    angle = ((msb << 8) | lsb) & ANGLE_MASK;
    return true;
}

int Encoder::readRotation(int motor) {
    if (motor < 0 || motor > 2) {
        Serial.println("Invalid motor selection");
        return -1; // Error value
    }

    switchEncoder(motor);

    uint16_t rawAngle = 0;
    if (!readRawAngle(rawAngle)) {
        Serial.println("Failed to read angle");
        return -1; // Error value
    }

    return rawAngle;
}

signed long long int Encoder::getCumPosition(int motor) {
    if (motor < 0 || motor > 2) {
        Serial.println("Invalid motor selection");
        return -1; // Error value
    }

    switchEncoder(motor);

    uint16_t rawAngle = 0;
    if (!readRawAngle(rawAngle)) {
        Serial.println("Failed to read angle");
        return -1; // Error value
    }

    _lastReadAngle[motor] = rawAngle;
    auto value = _lastReadAngle[motor];

    //  whole rotation CW?
    //  less than half a circle
    if ((_lastPosition[motor] > 2048) && ( value < (_lastPosition[motor] - 2048)))
    {
        _position[motor] = _position[motor] + 4096 - _lastPosition[motor] + value;
    }
    //  whole rotation CCW?
    //  less than half a circle
    else if ((value > 2048) && ( _lastPosition[motor] < (value - 2048)))
    {
        _position[motor] = _position[motor] - 4096 - _lastPosition[motor] + value;
    }
    else
    {
        _position[motor] = _position[motor] - _lastPosition[motor] + value;
    }
    _lastPosition[motor] = value;

    return _position[motor];
}

void Encoder::setCumPosition() {
    for (int i = 0; i < 3; i++) {
        switchEncoder(i);

        uint16_t value;
        readRawAngle(value);
        _lastPosition[i] = value;
        _position[i] = 0;
    }
}

