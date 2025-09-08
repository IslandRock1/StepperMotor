
#ifndef STEPPERMOTOR_ESPNOW_HPP
#define STEPPERMOTOR_ESPNOW_HPP


#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

typedef struct StepperData {
	uint16_t version;
	uint16_t motorID;
	int32_t rots;
	uint16_t acceleration_steps;
	uint16_t start_time;
	uint16_t min_time;
	uint16_t precision_rots;

	uint16_t is_finished;
	uint16_t NO_MOVE;
	uint32_t packet_id;
	uint16_t motor_signature_id;
	int16_t checksum;
} StepperData;

extern StepperData stepper_data;
extern StepperData retrieving_data;
extern int my_id;
extern bool recieved_new_data;

void on_data_sent(const uint8_t *macAdr, esp_now_send_status_t status);
void on_data_recv(const uint8_t *mac, const uint8_t *incomingData, int len);

class ESPNOW {
public:
	ESPNOW();
	void begin();
	void printAdr() const;
	void send_data() const;
	static bool get_is_new_data();
	static void reset_is_new_data();
	static StepperData get_stepper_data();

	static int16_t calcChecksum();

private:
	std::array<uint8_t, 6> _address{168, 66, 227, 174, 228, 4}; //
	esp_now_peer_info _peer_info;
	uint8_t _base_mac[6];
	bool _is_finished = true;
};


#endif //STEPPERMOTOR_ESPNOW_HPP