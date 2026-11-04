#pragma once
#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>
#include "secrets.h"

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
    static constexpr const bool isDebugEnabled = true;
    static constexpr const uint8_t BUILT_IN_LED_PIN = 15;

    static constexpr const uint8_t VOLTAGE_CHECK_INTERVAL = 10;
    static constexpr const uint8_t STATUS_SEND_INTERVAL = 25;
    static constexpr const uint16_t TICK_INTERVAL = 500;
    static constexpr const uint32_t DEBUG_TICK_INTERVAL = 5000;
    static constexpr const uint32_t WIFI_CHECK_INTERVAL = 30000;

    static constexpr const uint8_t NodeNumberPins[8] = {39, 40, 37, 38, 18, 21, 16, 17};

    // ===== VARIABLES =====
    uint32_t globalTime;

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
    uint32_t lastDebugTick = 0;

    bool wifiOTAEnabled = false;
    bool wifiServicesStarted = false;
    uint8_t lastWiFiCommand = 0;
    uint32_t lastWiFiCheck = 0;

    typedef struct __attribute__((packed))
    {
        uint8_t type;
        uint8_t node;
        uint8_t ttl;

        uint32_t globalTime;
        uint32_t commandTimestamp;

        uint8_t command;
        uint8_t parameter;

        uint8_t constantCommands[12];

        union __attribute__((packed))
        {
            struct __attribute__((packed))
            {
                uint16_t voltage;
                uint8_t charge;
            } status;
            struct __attribute__((packed))
            {
                uint8_t flags;
                uint16_t constantCommandMask;
            } command;
        } metadata;

        uint8_t targetBitmap[32];
        uint8_t reserved[182];
        uint8_t authTag[8];

    } Packet;

    static_assert(sizeof(Packet) == 250, "SPACE packet layout must be 250 bytes");
    static_assert(offsetof(Packet, targetBitmap) == 28, "Target bitmap offset must be 28");
    static_assert(offsetof(Packet, authTag) == 242, "Authentication tag offset must be 242");

    static constexpr uint8_t COMMAND_UPDATE_STATE = 0x01;
    static constexpr uint8_t COMMAND_UPDATE_CONSTANTS = 0x02;

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
    bool enableWiFiOTA(bool isShouldBeEnabled);
    void checkWiFiConnection();
    void debugOutput(const uint8_t *mac_addr = nullptr, const uint8_t *incomingData = nullptr, int len = 0);
    void parseReceviedData(const uint8_t *mac_addr, const uint8_t *incomingData, int len);
    void executeCommand(uint8_t nodeNumber);
    uint32_t getGlobalTime();
    uint8_t getNodeNumber();
};

extern SUNClass SUN;
