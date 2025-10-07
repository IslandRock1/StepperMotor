#ifndef STEPPERMOTOR_ENCODER_HPP
#define STEPPERMOTOR_ENCODER_HPP

#include <Wire.h>
#include <AS5600.h>

class SensorAS5600 {
public:
    SensorAS5600(int SDA, int SCL, int bus_num);
    void begin();
    int32_t getCumulativePosition();
    void resetCumulativePosition();

    static constexpr double RAW_TO_RAD = 2.0 * 3.1415926 / 4096.0;
    static constexpr double RAW_TO_DEG = 360.0 / 4096.0;
private:
    AS5600 _sensor;
    int _sda;
    int _scl;
    TwoWire _wire;
};

#endif //STEPPERMOTOR_ENCODER_HPP