#pragma once
#include <stdint.h>

namespace SOLARIS
{
    static constexpr uint16_t BREATHE_INTERVAL_1 = 250;
    static constexpr uint16_t BREATHE_INTERVAL_2 = 2500;
    static constexpr uint16_t BUTTON_PRESS_DELAY = 750;

    static constexpr uint8_t INNER_PWM_PIN = 1;
    static constexpr uint8_t MIDDLE_PWM_PIN = 2;
    static constexpr uint8_t OUTER_PWM_PIN = 4;
    static constexpr uint8_t EXTRA_PWM_PIN = 6;

    static constexpr uint8_t interval = 25;
}

class SolarisClass
{
public:
    // Solaris Specific methods
    uint8_t getInnerDataPin(uint8_t nodeNumber);
    uint8_t getMiddleDataPin(uint8_t nodeNumber);
    uint8_t getOuterDataPin(uint8_t nodeNumber);
    uint8_t getExtraDataPin(uint8_t nodeNumber);

    uint8_t getInnerStripPixelsQuantity(uint8_t nodeNumber);
    uint8_t getMiddleStripPixelsQuantity(uint8_t nodeNumber);
    uint8_t getOuterStripPixelsQuantity(uint8_t nodeNumber);
    void setupNode(uint8_t nodeNumber);
    void executeCommand(uint8_t nodeNumber);
};

extern class SolarisClass Solaris;