#pragma once

#include <FastLED.h>
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

    static constexpr uint8_t INTERVAL = 25;
    static constexpr uint16_t CROSS_FADE_INTERVAL = 3000;
    static constexpr uint8_t MIN_BRIGHT = 16;
    static constexpr uint8_t MAX_BRIGHT = 192;
    static constexpr uint8_t SAFE_CHARGE = 75;
    static constexpr uint8_t HANDS_MAX_BRIGHT = 255;
    static constexpr uint8_t INNER_MAX_PIXELS = 55;
    static constexpr uint8_t MIDDLE_MAX_PIXELS = 62;
    static constexpr uint8_t OUTER_MAX_PIXELS = 98;
    static constexpr uint8_t EXTRA_MAX_PIXELS = 10;
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
    uint8_t getExtraStripPixelsQuantity(uint8_t nodeNumber);
    void setupNode(uint8_t nodeNumber);
    void executeCommand(uint8_t nodeNumber);

private:
    CRGB innerStrip[SOLARIS::INNER_MAX_PIXELS]{};
    CRGB middleStrip[SOLARIS::MIDDLE_MAX_PIXELS]{};
    CRGB outerStrip[SOLARIS::OUTER_MAX_PIXELS]{};
    CRGB extraStrip[SOLARIS::EXTRA_MAX_PIXELS]{};
    uint8_t bright = SOLARIS::MIN_BRIGHT;
    uint16_t innerCount = 0;
    uint16_t middleCount = 0;
    uint16_t outerCount = 0;
    uint16_t extraCount = 0;

    uint32_t elapsedSinceCommand() const;
    bool registerStrip(CRGB *pixels, uint16_t count, uint8_t dataPin);
    void writeStrip(CRGB *pixels, uint16_t count, uint8_t brightness);
    void strobe(uint32_t interval, bool state);
    void crossFade(uint32_t duration, bool state);
    void variableCrossFade(uint32_t duration, uint32_t interval);
    void onOff(bool state);
    void breathe(uint32_t inhaleDuration, uint32_t exhaleDuration);
    void breatheSingle(uint32_t inhaleDuration, uint32_t exhaleDuration);
    void breatheHard(uint32_t inhaleDuration, uint32_t exhaleDuration);
    void fadeParameter(uint8_t startValue, uint8_t endValue, uint32_t duration);
    void fadeToStrobe(uint32_t duration, uint32_t interval);
    void setBrightness(uint8_t brightness);
    uint8_t handsValue(uint8_t input) const;
};

extern SolarisClass Solaris;
