

#include <esp_wifi.h>

#include "ESPNOW.hpp"
#include "stepper.h"

StepperData stepper_data;
StepperData retrieving_data;
int my_id = 0;
bool recieved_new_data = false;

void on_data_sent(const uint8_t* macAdr, const esp_now_send_status_t status) {
	if (status != ESP_NOW_SEND_SUCCESS) {
		Serial.println("Sent data not succ");
	}
}

void on_data_recv(const uint8_t* mac, const uint8_t* incomingData, int len) {
	memcpy(&retrieving_data, incomingData, sizeof(retrieving_data));
	recieved_new_data = true;
}

ESPNOW::ESPNOW() {
	_peer_info.channel = 0;
	_peer_info.encrypt = false;
	_base_mac;
}

void ESPNOW::begin() {
	WiFi.mode(WIFI_STA);
	if (esp_now_init() != ESP_OK) {
		Serial.println("ESP_Now does not work..");
	}

	auto ret = esp_wifi_get_mac(WIFI_IF_STA, _base_mac);
	if (ret != ESP_OK) {
		Serial.println("Unable to retrieve base_mac...");
	}
	if (_base_mac[0] == 0xe4) {my_id = 1;}
	else if (_base_mac[0] == 0xa8) {my_id = 2;}
	else {
		Serial.println("Something wrong with basemac?");
	}

	esp_now_register_send_cb(on_data_sent);
	esp_now_register_recv_cb(on_data_recv);

	memcpy(_peer_info.peer_addr, _address.data(), 6);
	ret = esp_now_add_peer(&_peer_info);
	if (ret != ESP_OK) {
		Serial.println("Failed to register address0 for ESP_Now");
	}
}

void ESPNOW::send_data() const {
	esp_err_t result = esp_now_send(_address.data(), reinterpret_cast<uint8_t*>(&stepper_data), sizeof(stepper_data));

	if (result != ESP_NOW_SEND_SUCCESS) {
		Serial.println("Turnt not good");
	}
}

bool ESPNOW::get_is_new_data() {
	return recieved_new_data;
}
void ESPNOW::reset_is_new_data() {
	recieved_new_data = false;
}

void ESPNOW::printAdr() const {
	for (int i = 0; i < 5; i++) {
		Serial.print(_base_mac[i]);
		Serial.print(":");
	}
	Serial.println(_base_mac[5]);
}

StepperData ESPNOW::get_stepper_data() {
	return stepper_data;
}
