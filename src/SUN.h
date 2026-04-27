#pragma once
#include <Arduino.h>
#include <stdint.h>

#include "../src/NodeSpecific/Origami.h"
#include "../src/NodeSpecific/Solaris.h"
#include "../src/NodeSpecific/MoonFaced.h"
#include "../src/NodeSpecific/Snow.h"
#include "../src/NodeSpecific/Cat.h"
#include "../src/NodeSpecific/SkyStranger.h"
#include "../src/NodeSpecific/Animoll.h"

class SUNClass
{
public:
    // ===== CONSTANTS =====
    static constexpr const uint8_t BUILT_IN_LED_PIN = 15;

    static constexpr const uint8_t VOLTAGE_CHECK_INTERVAL = 10;
    static constexpr const uint8_t STATUS_SEND_INTERVAL = 25;
    static constexpr const uint16_t TICK_INTERVAL = 500;

    static constexpr const uint8_t DEFAULT_WIFI_CHANNEL = 13;

    static constexpr const uint8_t NodeNumberPins[8] = {39, 40, 37, 38, 18, 21, 16, 17};

    static constexpr const char *ssid = "SvetlitsaNet";
    static constexpr const char *password = "gggggggggg";

    // ===== VARIABLES =====
    uint16_t globalTime;

    char *hostName;
    uint8_t nodeNumber;

    uint8_t TTL = 3;
    int32_t globalTimeOffset;

    uint8_t command;
    uint8_t parameter;
    uint8_t constantCommands[12];
    uint32_t startMillis;

    uint8_t voltageReadCounter;
    uint16_t voltage;
    uint8_t charge;
    uint16_t voltageBuffer[256];

    uint32_t lastCheckVoltage = 0;
    uint32_t lastSendStatus = 0;

    uint32_t lastTick = 0;

    typedef struct __attribute__((packed))
    {
        uint8_t type;
        uint8_t ttl;
        uint8_t node;

        uint32_t globalTime;
        uint32_t commandTimestamp;

        uint16_t voltage;
        uint8_t charge;

        uint8_t command;
        uint8_t parameter;

        uint8_t constantCommands[12];

        uint8_t payload_size;
        uint8_t payload[71];

    } Packet;

    uint16_t getVoltageIndexer(uint8_t nodeNumber);
    void setNodeCommand(uint8_t nodeNumber, uint8_t command, uint8_t parameter, uint8_t constantParameters[12]);
    void setupNode(uint8_t nodeNumber);
    String getHostName(uint8_t nodeNumber);
    uint8_t getADCPin(uint8_t nodeNumber);
    uint8_t getCharge(uint8_t nodeNumber);
    uint16_t getLowVoltage(uint8_t nodeNumber);
    uint16_t getHighVoltage(uint8_t nodeNumber);
    void sendStatus(uint8_t nodeNumber);
    bool sendMessage(const uint8_t *payload, size_t payloadSize);
    void parseReceviedData(const uint8_t *mac_addr, const uint8_t *incomingData, int len);
    uint32_t getGlobalTime();
    uint8_t getNodeNumber();
};

extern SUNClass SUN;
