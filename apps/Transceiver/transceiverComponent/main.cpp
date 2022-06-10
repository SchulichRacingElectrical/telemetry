// Copyright Schulich Racing, FSAE
// Written by Jon Mulyk and Justin Tijunelis

#include "legato.h"
#include <thread>
#include <iostream>
#include <optional>
#include <vector>
#include "can.hpp"
#include "constants.h"
#include "transceiver.hpp"
#include "vfdcp_encoder.hpp"

COMPONENT_INIT {
	// Connect to LTE and start the CAN driver
    system("sh /home/root/start_connect.sh &");
    for (int i = 0; i < 3; i++) system("sh /home/root/start_can.sh red &");

    // Attempt to fetch the sensors from the server
    Transceiver transceiver = Transceiver(SERIAL_NUMBER, API_KEY, WEB_SERVER_ENDPOINT);
    std::optional<std::vector<Sensor>> sensors = std::nullopt; 
    while (!sensors.has_value()) {
        sensors = transceiver.fetchSensors();
    }

    // If there are no sensors, there is no point in continuing
    if (sensors.value().size() == 0) return;

    // Attempt to start the CAN bus
    std::cout << "Starting can" << std::endl;
    CanBus canBus = CanBus(sensors.value());
    while (!canBus.initialize());
    canBus.open();

    // Attempt to start a session with the server
    std::cout << "requesting session" << std::endl;
    while (!transceiver.requestSession());
    transceiver.initializeUdp();

    // Start the CAN reading thread
    auto callback = [&](unsigned int timestamp, std::vector<SensorVariantPair> data) {
        std::vector<unsigned char> bytes = encode_data(timestamp, data);
        transceiver.sendVfdcpData(bytes);
    };
    std::cout << "Starting can" << std::endl;
    canBus.readAndTrigger(callback);
}