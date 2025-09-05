
#include <Arduino.h>
#include <Wire.h>
#include "Encoder.hpp"

Encoder::Encoder(int scl, int sda)
    : scl(scl), sda(sda) {}

void Encoder::begin() {
    Wire.begin(sda, scl);
}

int Encoder::readRotation() const {
    Wire.beginTransmission(address);
    Wire.write(raw_angle_ms);
    Wire.endTransmission(false);  // Repeated start
    Wire.requestFrom(address, 2);

    if (Wire.available() < 2) {
        Serial.println("Error: Couldn't read angle");
        return 0;
    }

    const uint8_t msb = Wire.read();
    const uint8_t lsb = Wire.read();

    return ((msb << 8) | lsb) & 0x0FFF;
}

