#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <ArduinoOTA.h>
#if __has_include(<esp_idf_version.h>)
#include <esp_idf_version.h>
#endif

// Include main class
#include "SUN.h"

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  SUN.parseReceviedData(mac, incomingData, len);
}

void setup()
{
  Serial.begin(115200);

  SUN.nodeNumber = SUN.getNodeNumber();

  Serial.print("Starting configurations as ");
  Serial.println(SUN.getHostName(SUN.nodeNumber));

  SUN.setupNode(SUN.nodeNumber);

  Serial.print("Configured and starting as ");
  Serial.println(SUN.getHostName(SUN.nodeNumber));

  esp_err_t registerResult = esp_now_register_recv_cb(OnDataRecv);
  if (registerResult != ESP_OK)
  {
    Serial.printf("ESP-NOW recv callback registration failed: %d\n", registerResult);
  }
  else
  {
    Serial.println("ESP-NOW recv callback registered");
  }
}

void loop()
{
  ArduinoOTA.handle();

  if (millis() - SUN.lastCheckVoltage > SUNClass::VOLTAGE_CHECK_INTERVAL)
  {
    SUN.lastCheckVoltage = millis();
    SUN.charge = SUN.getCharge(SUN.nodeNumber);
  }

  if (millis() - SUN.lastSendStatus > SUNClass::STATUS_SEND_INTERVAL)
  {
    SUN.lastSendStatus = millis();
    SUN.sendStatus(SUN.nodeNumber);
  }

  if (SUN.getGlobalTime() - SUN.lastTick > SUNClass::TICK_INTERVAL)
  {
    SUN.lastTick = SUN.getGlobalTime();
    bool phase = (SUN.getGlobalTime() / SUNClass::TICK_INTERVAL) & 1;
    analogWrite(SUN.BUILT_IN_LED_PIN, phase ? 4 : 0);

    // Serial.printf("Tick\n");
  }

  uint32_t currentGlobalTime = SUN.getGlobalTime();
  if (currentGlobalTime - SUN.lastDebugTick >= SUNClass::DEBUG_TICK_INTERVAL)
  {
    SUN.lastDebugTick = currentGlobalTime;
    SUN.debugOutput();
  }
}
