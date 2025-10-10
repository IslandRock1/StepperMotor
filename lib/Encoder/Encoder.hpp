#ifndef STEPPERMOTOR_ENCODER_HPP
#define STEPPERMOTOR_ENCODER_HPP

#include <Arduino.h>
#include <Wire.h>

class Encoder {
public:
    Encoder(int scl, int sda, int A, int B);

    void begin();
    int readRotation(int motor); // motor = 0, 1, or 2
    signed long long int getCumPosition(int motor);
    void setCumPosition();

private:
    static constexpr uint8_t ADDRESS = 0x36;
    static constexpr uint8_t RAW_ANGLE_MSB = 0x0C;
    static constexpr uint8_t RAW_ANGLE_LSB = 0x0D;
    static constexpr uint16_t ANGLE_MASK = 0x0FFF; // 12-bit resolution

    void switchEncoder(int motor);
    bool readRawAngle(uint16_t &angle);

    signed long long int _lastReadAngle[3] = {0, 0, 0};
    signed long long int _position[3] = {0, 0, 0};
    signed long long int _lastPosition[3] = {0, 0, 0};


    int scl_pin, sda_pin, pinA, pinB;
};


#endif //STEPPERMOTOR_ENCODER_HPP