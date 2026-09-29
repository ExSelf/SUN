#ifndef ARDUINO_H
#include <Arduino.h>
#endif

#ifndef ARDUINOOTA_H
#include <ArduinoOTA.h>
#endif

#ifndef ESP_NOW_H
#include <esp_now.h>
#endif

#ifndef ESP_WIFI_H
#include <esp_wifi.h>
#endif

#ifndef ESP_MDNS_H
#include <ESPmDNS.h>
#endif

#include "SUN.h"
#include "string.h"

// Create global instance
SUNClass SUN;
SUNClass::Packet packet;

constexpr const uint8_t SUNClass::NodeNumberPins[8];

uint16_t SUNClass::getVoltageIndexer(uint8_t nodeNumber)
{
    switch (nodeNumber)
    {
    case 12:
        return 730;

    default:
        return 0;
    }
}

void SUNClass::setNodeCommand(uint8_t nodeNumber, uint8_t command, uint8_t parameter, uint8_t constantParameters[12])
{
    Serial.printf("Send command %d to node %d\n", command, nodeNumber);
}

void SUNClass::setupNode(uint8_t nodeNumber)
{
    this->nodeNumber = nodeNumber;
    analogReadResolution(12);

    if (nodeNumber != 255)
    {
        if (nodeNumber > 10) // Origami
        {
            Origami.setupNode(nodeNumber);
        }
        else if (nodeNumber > 20) // Solaris
        {
            Solaris.setupNode(nodeNumber);
        }
        else if (nodeNumber > 30) // MoonFaced
        {
            MoonFaced.setupNode(nodeNumber);
        }
        else if (nodeNumber > 40) // Snow
        {
            Snow.setupNode(nodeNumber);
        }
        else if (nodeNumber > 50 && nodeNumber < 60) // Cat
        {
            Cat.setupNode(nodeNumber);
        }
        else if (nodeNumber > 70) // SkyStranger
        {
            SkyStranger.setupNode(nodeNumber);
        }
        else if (nodeNumber == 201) // Animoll
        {
            Animoll.setupNode(nodeNumber);
        }
    }

    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(DEFAULT_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
    WiFi.hostname(SUN.getHostName(nodeNumber).c_str());

    Serial.print("Device is being configured as number ");
    Serial.print(String(nodeNumber));
    Serial.print(" with hostname ");
    Serial.println(SUN.getHostName(nodeNumber));

    pinMode(SUN.getADCPin(nodeNumber), INPUT);
    pinMode(BUILT_IN_LED_PIN, OUTPUT);
    digitalWrite(BUILT_IN_LED_PIN, HIGH);

    // fill voltage buffer with initial values
    uint16_t currentVoltage = analogRead(SUN.getADCPin(nodeNumber));
    for (int i = 0; i < 256; i++)
    {
        voltageBuffer[i] = currentVoltage * SUN.getVoltageIndexer(nodeNumber) / 100;
    }

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW init failed");
        return;
    }
}

bool SUNClass::enableWiFiOTA(bool isShouldBeEnabled)
{
    if (isShouldBeEnabled)
    {
        if (wifiOTAEnabled)
        {
            return true; // Already enabled
        }

        WiFi.mode(WIFI_STA);
        esp_wifi_set_channel(DEFAULT_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        WiFi.hostname(getHostName(nodeNumber).c_str());

        if (!MDNS.begin(getHostName(nodeNumber).c_str()))
        {
            Serial.println("Error starting mDNS");
        }
        else
        {
            Serial.println("mDNS started");
        }

        ArduinoOTA.setHostname(getHostName(nodeNumber).c_str());
        ArduinoOTA.onStart([]()
                           { Serial.println("OTA Start"); });
        ArduinoOTA.onEnd([]()
                         { Serial.println("\nOTA End"); });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                              { Serial.printf("Progress: %u%%\r", (progress * 100) / total); });
        ArduinoOTA.onError([](ota_error_t error)
                           { Serial.printf("Error[%u]\n", error); });
        ArduinoOTA.begin();

        Serial.println();
        Serial.print("Connected. IP: ");
        Serial.println(WiFi.localIP());

        wifiOTAEnabled = true;
        return true;
    }
    else
    {
        if (!wifiOTAEnabled)
        {
            return true; // Already disabled
        }

        WiFi.disconnect(true); // true = turn off WiFi radio
        MDNS.end();
        ArduinoOTA.end();

        Serial.println("WiFi/OTA disabled");
        wifiOTAEnabled = false;
        return true;
    }
}

String SUNClass::getHostName(uint8_t nodeNumber)
{
    if (nodeNumber == 255)
    {
        return "Debug";
    }
    else if (nodeNumber >= 10 && nodeNumber < 20)
    {
        return "Origami_" + String(nodeNumber);
    }
    else if (nodeNumber < 30)
    {
        return "Solaris_" + String(nodeNumber % 10);
    }
    else if (nodeNumber < 40)
    {
        return "MoonFaced_" + String(nodeNumber % 10);
    }
    else
    {
        return "UnknownNode_" + String(nodeNumber % 10);
    }
}

uint8_t SUNClass::getADCPin(uint8_t nodeNumber)
{
    {
        switch (nodeNumber)
        {

        default:
            return 5;
        }
    }
}

uint8_t SUNClass::getCharge(uint8_t nodeNumber)
{
    voltageReadCounter++;
    voltageBuffer[voltageReadCounter] = analogRead(SUN.getADCPin(nodeNumber)) * SUN.getVoltageIndexer(nodeNumber) / 100;

    uint32_t total = 0;
    for (uint16_t i = 0; i <= 255; i++)
    {
        total += voltageBuffer[i];
    }
    voltage = total / 256;

    uint8_t charge = constrain(map(voltage, SUN.getLowVoltage(nodeNumber), SUN.getHighVoltage(nodeNumber), 0, 100), 0, 100);

    return charge;

    // bright = constrain(map(charge, 0, SAFE_CHARGE, MIN_BRIGHT, MAX_BRIGHT), MIN_BRIGHT, MAX_BRIGHT);
}

uint16_t SUNClass::getLowVoltage(uint8_t nodeNumber)
{
    switch (nodeNumber)
    {
    case 21:
        return 9000;

    case 22:
        return 9000;

    case 23:
        return 9000;

    case 24:
        return 9000;

    case 25:
        return 9000;

    default:
        return 0;
    }
}

uint16_t SUNClass::getHighVoltage(uint8_t nodeNumber)
{
    switch (nodeNumber)
    {

    case 21:
        return 12600;

    case 22:
        return 12600;

    case 23:
        return 12600;

    case 24:
        return 12600;

    case 25:
        return 12600;

    default:
        return 65535;
    }
}

void SUNClass::sendStatus(uint8_t nodeNumber)
{
    Packet packet{}; // ✅ zero-initialize everything

    // ===== HEADER =====
    packet.type = 2; // STATUS
    packet.node = nodeNumber;
    packet.ttl = TTL;

    packet.globalTime = SUN.getGlobalTime();
    packet.commandTimestamp = startMillis;

    packet.voltage = voltage;
    packet.charge = charge;

    packet.command = command;
    packet.parameter = parameter;

    // ===== COPY ARRAY =====
    memcpy(packet.constantCommands,
           constantCommands,
           sizeof(packet.constantCommands));

    if (SUN.sendMessage(reinterpret_cast<const uint8_t *>(&packet), sizeof(packet)))
    {
        // Serial.println("Status message sent successfully");
    }
    else
    {
        Serial.println("Failed to send status message");
    }
}

bool SUNClass::sendMessage(const uint8_t *payload, size_t payloadSize)
{
    static const uint8_t broadcastMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    if (payload == nullptr || payloadSize == 0)
    {
        Serial.println("ESP-NOW broadcast skipped: invalid payload");
        return false;
    }

    if (payloadSize > ESP_NOW_MAX_DATA_LEN)
    {
        Serial.printf("ESP-NOW broadcast skipped: payload too large (%u > %u)\n", (unsigned int)payloadSize, ESP_NOW_MAX_DATA_LEN);
        return false;
    }

    if (!esp_now_is_peer_exist(broadcastMac))
    {
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, broadcastMac, 6);
        // Keep channel in sync with current STA channel (important when WiFi is connected to AP).
        peerInfo.channel = DEFAULT_WIFI_CHANNEL;
        peerInfo.ifidx = WIFI_IF_STA;
        peerInfo.encrypt = false;

        esp_err_t addResult = esp_now_add_peer(&peerInfo);
        if (addResult != ESP_OK)
        {
            Serial.printf("ESP-NOW add broadcast peer failed: %d\n", addResult);
            return false;
        }
    }

    esp_err_t sendResult = esp_now_send(broadcastMac, payload, payloadSize);
    if (sendResult != ESP_OK)
    {
        Serial.printf("ESP-NOW broadcast send failed, err=%d\n", sendResult);
        return false;
    }

    return true;
}

void SUNClass::debugOutput(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
    if (!isDebugEnabled)
    {
        return;
    }

    if (mac_addr == nullptr && incomingData == nullptr && len == 0)
    {
        Serial.printf("Debug tick: global time=%lu ms\n", (unsigned long)getGlobalTime());
        return;
    }

    if (mac_addr != nullptr)
    {
        Serial.printf("ESP-NOW RX from %02X:%02X:%02X:%02X:%02X:%02X size=%d",
                      mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5], len);
    }
    else
    {
        Serial.printf("ESP-NOW RX size=%d", len);
    }

    if (incomingData != nullptr && len >= 3)
    {
        Serial.printf(" type=%u node=%u ttl=%u", incomingData[0], incomingData[1], incomingData[2]);
        if (len >= 13)
        {
            Serial.printf(" command=%u parameter=%u", incomingData[11], incomingData[12]);
        }
    }
    Serial.println();
}

void SUNClass::parseReceviedData(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
    debugOutput(mac_addr, incomingData, len);

    if (mac_addr == nullptr || incomingData == nullptr || len <= 0)
    {
        return;
    }

    if (len != (int)sizeof(Packet))
    {
        Serial.printf("ESP-NOW RX incorrect: %d (need %u)\n", len, (unsigned int)sizeof(Packet));
        return;
    }

    Packet receivedPacket{};
    memcpy(&receivedPacket, incomingData, sizeof(Packet));

    // Read and apply fields from received packet.
    // GLOBAL::TTL = receivedPacket.ttl;
    // GLOBAL::command = receivedPacket.command;
    // GLOBAL::parameter = receivedPacket.parameter;
    // GLOBAL::voltage = receivedPacket.voltage;
    // GLOBAL::charge = receivedPacket.charge;
    // memcpy(GLOBAL::constantCommands, receivedPacket.constantCommands, sizeof(GLOBAL::constantCommands));

    if (globalTimeOffset < receivedPacket.globalTime - millis())
    {
        globalTimeOffset = (int32_t)receivedPacket.globalTime - (int32_t)millis();
        Serial.printf("Time offset adjusted: %d ms\n", globalTimeOffset);
    }

    // Handle WiFi/OTA control via constantCommands[0]
    // Only trigger on command change to avoid repeated calls
    if (receivedPacket.constantCommands[0] != lastWiFiCommand)
    {
        lastWiFiCommand = receivedPacket.constantCommands[0];
        bool shouldEnable = (receivedPacket.constantCommands[0] != 0);
        enableWiFiOTA(shouldEnable);
        Serial.printf("WiFi/OTA %s via command: %u\n", shouldEnable ? "enabled" : "disabled", receivedPacket.constantCommands[0]);
    }

}

uint32_t SUNClass::getGlobalTime()
{
    return millis() + globalTimeOffset;
}

uint8_t SUNClass::getNodeNumber()
{
    uint8_t node = 0;
    for (int i = 0; i < 8; i++)
    {
        pinMode(NodeNumberPins[i], INPUT_PULLUP);
        node |= (uint8_t)((!digitalRead(NodeNumberPins[i])) << i);
    }
    return node == 0 ? 255 : node;
}
