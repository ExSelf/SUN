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
#include <stddef.h>
#include <mbedtls/md.h>

#ifndef SUN_PACKET_HMAC_KEY
#error "Define SUN_PACKET_HMAC_KEY in src/secrets.h"
#endif

// Create global instance
SUNClass SUN;
SUNClass::Packet packet;

constexpr const uint8_t SUNClass::NodeNumberPins[8];

static constexpr size_t AUTH_TAG_SIZE = 8;
static constexpr size_t AUTHENTICATED_PACKET_SIZE = offsetof(SUNClass::Packet, authTag);
static_assert(sizeof(SUNClass::Packet::authTag) == AUTH_TAG_SIZE, "HMAC tag must be 8 bytes");

static bool calculatePacketAuthTag(const uint8_t *data, uint8_t *authTag)
{
    static const char key[] = SUN_PACKET_HMAC_KEY;
    static_assert(sizeof(key) - 1 >= 32, "SUN_PACKET_HMAC_KEY must be at least 32 characters");

    const mbedtls_md_info_t *sha256 = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    uint8_t digest[32];
    if (sha256 == nullptr ||
        mbedtls_md_hmac(sha256,
                        reinterpret_cast<const unsigned char *>(key),
                        sizeof(key) - 1,
                        data,
                        AUTHENTICATED_PACKET_SIZE,
                        digest) != 0)
    {
        return false;
    }

    memcpy(authTag, digest, AUTH_TAG_SIZE);
    return true;
}

static bool hasValidPacketAuthTag(const SUNClass::Packet &packet)
{
    uint8_t expected[AUTH_TAG_SIZE];
    if (!calculatePacketAuthTag(reinterpret_cast<const uint8_t *>(&packet), expected))
    {
        return false;
    }

    uint8_t difference = 0;
    for (size_t i = 0; i < AUTH_TAG_SIZE; ++i)
    {
        difference |= expected[i] ^ packet.authTag[i];
    }
    return difference == 0;
}

static bool finalizePacket(SUNClass::Packet &packet)
{
    return calculatePacketAuthTag(
        reinterpret_cast<const uint8_t *>(&packet),
        packet.authTag);
}

uint16_t SUNClass::getVoltageIndexer(uint8_t nodeNumber)
{
    switch (nodeNumber)
    {
    case 21:
        return 188;
    case 22:
        return 188;
    case 23:
        return 188;
    case 24:
        return 188;
    case 25:
        return 188;

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
    // analogReadResolution(12);

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
    enableWiFiOTA(true);

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
        WiFi.hostname(getHostName(nodeNumber).c_str());

        wifiOTAEnabled = true;
        wifiServicesStarted = false;
        WiFi.setAutoReconnect(true);
        lastWiFiCheck = millis();
        Serial.printf("Connecting to WiFi SSID \"%s\" on channel %u\n", WIFI_SSID, DEFAULT_WIFI_CHANNEL);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD, DEFAULT_WIFI_CHANNEL);
        return true;
    }
    else
    {
        if (!wifiOTAEnabled)
        {
            return true; // Already disabled
        }

        if (wifiServicesStarted)
        {
            MDNS.end();
            ArduinoOTA.end();
            wifiServicesStarted = false;
        }
        WiFi.disconnect(true); // true = turn off WiFi radio

        Serial.println("WiFi/OTA disabled");
        wifiOTAEnabled = false;
        return true;
    }
}

void SUNClass::checkWiFiConnection()
{
    if (!wifiOTAEnabled)
    {
        return;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        if (!wifiServicesStarted)
        {
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
            wifiServicesStarted = true;

            Serial.print("Connected. IP: ");
            Serial.println(WiFi.localIP());
        }
        return;
    }

    if (wifiServicesStarted)
    {
        MDNS.end();
        ArduinoOTA.end();
        wifiServicesStarted = false;
        Serial.println("WiFi disconnected; waiting to reconnect");
    }

    if (millis() - lastWiFiCheck < WIFI_CHECK_INTERVAL)
    {
        return;
    }
    lastWiFiCheck = millis();

    Serial.printf("WiFi disconnected; retrying SSID \"%s\" on channel %u\n", WIFI_SSID, DEFAULT_WIFI_CHANNEL);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD, DEFAULT_WIFI_CHANNEL);
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

    packet.metadata.status.voltage = voltage;
    packet.metadata.status.charge = charge;

    packet.command = command;
    packet.parameter = parameter;

    // ===== COPY ARRAY =====
    memcpy(packet.constantCommands,
           constantCommands,
           sizeof(packet.constantCommands));

    if (!finalizePacket(packet))
    {
        Serial.println("Status packet authentication failed");
        return;
    }

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
        // Serial.printf("ESP-NOW RX incorrect: %d (need %u)\n", len, (unsigned int)sizeof(Packet));
        return;
    }

    Packet receivedPacket{};
    memcpy(&receivedPacket, incomingData, sizeof(Packet));

    if (!hasValidPacketAuthTag(receivedPacket))
    {
        // Serial.println("ESP-NOW RX rejected: authentication failed");
        return;
    }

    if (receivedPacket.type != 1 || receivedPacket.ttl == 0 || receivedPacket.ttl > 12 ||
        (receivedPacket.metadata.command.flags &
         ~(COMMAND_UPDATE_STATE | COMMAND_UPDATE_CONSTANTS)) != 0 ||
        (receivedPacket.metadata.command.constantCommandMask & 0xF000) != 0)
    {
        return;
    }

    const uint8_t targetByte = nodeNumber / 8;
    const uint8_t targetMask = (uint8_t)(1U << (nodeNumber % 8));
    if ((receivedPacket.targetBitmap[targetByte] & targetMask) == 0)
    {
        return;
    }

    // The target node uses the packet timestamp as its receipt acknowledgement.
    TTL = receivedPacket.ttl;
    startMillis = receivedPacket.commandTimestamp;
    if (receivedPacket.metadata.command.flags & COMMAND_UPDATE_STATE)
    {
        command = receivedPacket.command;
        parameter = receivedPacket.parameter;
    }
    if (receivedPacket.metadata.command.flags & COMMAND_UPDATE_CONSTANTS)
    {
        for (uint8_t i = 0; i < sizeof(constantCommands); ++i)
        {
            if (receivedPacket.metadata.command.constantCommandMask & (1U << i))
            {
                constantCommands[i] = receivedPacket.constantCommands[i];
            }
        }
    }

    if (globalTimeOffset < receivedPacket.globalTime - millis())
    {
        globalTimeOffset = receivedPacket.globalTime - millis();
        Serial.printf("Time offset adjusted: %d ms\n", globalTimeOffset);
    }

    // Handle WiFi/OTA control via constantCommands[0]
    // Only trigger on command change to avoid repeated calls
    if ((receivedPacket.metadata.command.flags & COMMAND_UPDATE_CONSTANTS) &&
        (receivedPacket.metadata.command.constantCommandMask & 1U) &&
        constantCommands[0] != lastWiFiCommand)
    {
        lastWiFiCommand = constantCommands[0];
        bool shouldEnable = (constantCommands[0] != 0);
        enableWiFiOTA(shouldEnable);
        Serial.printf("WiFi/OTA %s via command: %u\n", shouldEnable ? "enabled" : "disabled", constantCommands[0]);
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

void SUNClass::executeCommand(uint8_t nodeNumber)
{
    if (nodeNumber == 255)
    {
        Serial.printf("No specific command execution for node %d\n", nodeNumber);
        return;
    }
    else if (nodeNumber >= 10 && nodeNumber < 20)
    {
        Origami.executeCommand(nodeNumber);
    }
    else if (nodeNumber >= 20 && nodeNumber < 30)
    {
        Solaris.executeCommand(nodeNumber);
    }
    else if (nodeNumber >= 30 && nodeNumber < 40)
    {
        MoonFaced.executeCommand(nodeNumber);
    }
    else if (nodeNumber >= 40 && nodeNumber < 50)
    {
        Snow.executeCommand(nodeNumber);
    }
    else if (nodeNumber >= 50 && nodeNumber < 60)
    {
        Cat.executeCommand(nodeNumber);
    }
    else if (nodeNumber >= 70)
    {
        SkyStranger.executeCommand(nodeNumber);
    }
    else if (nodeNumber == 201)
    {
        Animoll.executeCommand(nodeNumber);
    }
}
