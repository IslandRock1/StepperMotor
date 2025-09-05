
#ifndef STEPPERMOTOR_ENCODER_HPP
#define STEPPERMOTOR_ENCODER_HPP


class Encoder {
public:
    Encoder(int scl, int sda);
    void begin();
    int readRotation() const;
private:
    int address = 0x36;
    int raw_angle_ms = 0x0C;
    int raw_angle_ls = 0x0D;

    int scl;
    int sda;
};


#endif //STEPPERMOTOR_ENCODER_HPP