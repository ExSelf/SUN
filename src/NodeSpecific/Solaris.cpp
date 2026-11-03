#include <Arduino.h>
#include <math.h>
#include "Solaris.h"
#include "../SUN.h"

uint8_t SolarisClass::getInnerDataPin(uint8_t nodeNumber)
{
  switch (nodeNumber)
  {
  case 21:
    return 35;

  case 22:
    return 35;

  case 23:
    return 35;

  case 24:
    return 35;

  case 25:
    return 35;

  default:
    return 0;
  }
}

uint8_t SolarisClass::getMiddleDataPin(uint8_t nodeNumber)
{
  switch (nodeNumber)
  {
  case 21:
    return 36;

  case 22:
    return 36;

  case 23:
    return 36;

  case 24:
    return 36;

  case 25:
    return 36;

  default:
    return 0;
  }
}

uint8_t SolarisClass::getOuterDataPin(uint8_t nodeNumber)
{
  switch (nodeNumber)
  {
  case 21:
    return 33;

  case 22:
    return 33;

  case 23:
    return 34;

  case 24:
    return 34;

  case 25:
    return 34;

  default:
    return 0;
  }
}

uint8_t SolarisClass::getExtraDataPin(uint8_t nodeNumber)
{
  switch (nodeNumber)
  {
  case 21:
    return 34;

  case 22:
    return 34;

  case 23:
    return 33;

  case 24:
    return 33;

  case 25:
    return 33;

  default:
    return 0;
  }
}

uint8_t SolarisClass::getInnerStripPixelsQuantity(uint8_t nodeNumber)
{
  switch (nodeNumber)
  {
  case 21:
    return 31;

  case 22:
    return 31;

  case 23:
    return 34;

  case 24:
    return 55;

  case 25:
    return 55;

  default:
    return 0;
  }
}

uint8_t SolarisClass::getMiddleStripPixelsQuantity(uint8_t nodeNumber)
{
  switch (nodeNumber)
  {
  case 21:
    return 29;

  case 22:
    return 29;

  case 23:
    return 30;

  case 24:
    return 62;

  case 25:
    return 62;

  default:
    return 0;
  }
}

uint8_t SolarisClass::getOuterStripPixelsQuantity(uint8_t nodeNumber)
{
  switch (nodeNumber)
  {
  case 21:
    return 98;

  case 22:
    return 98;

  case 23:
    return 98;

  case 24:
    return 68;

  case 25:
    return 68;

  default:
    return 0;
  }
}

uint8_t SolarisClass::getExtraStripPixelsQuantity(uint8_t nodeNumber)
{
  switch (nodeNumber)
  {
  default:
    return 10;
  }
}

void SolarisClass::setupNode(uint8_t nodeNumber)
{
  pinMode(SOLARIS::INNER_PWM_PIN, OUTPUT);
  pinMode(SOLARIS::MIDDLE_PWM_PIN, OUTPUT);
  pinMode(SOLARIS::OUTER_PWM_PIN, OUTPUT);
  pinMode(SOLARIS::EXTRA_PWM_PIN, OUTPUT);

  const uint8_t innerPin = getInnerDataPin(nodeNumber);
  const uint8_t middlePin = getMiddleDataPin(nodeNumber);
  const uint8_t outerPin = getOuterDataPin(nodeNumber);
  const uint8_t extraPin = getExtraDataPin(nodeNumber);

  const uint8_t innerCount = getInnerStripPixelsQuantity(nodeNumber);
  const uint8_t middleCount = getMiddleStripPixelsQuantity(nodeNumber);
  const uint8_t outerCount = getOuterStripPixelsQuantity(nodeNumber);
  const uint8_t extraCount = getExtraStripPixelsQuantity(nodeNumber);

  if (!registerStrip(innerStrip, innerCount, innerPin) ||
      !registerStrip(middleStrip, middleCount, middlePin) ||
      !registerStrip(outerStrip, outerCount, outerPin) ||
      !registerStrip(extraStrip, extraCount, extraPin))
  {
    Serial.printf("Solaris node %u has an unsupported LED strip configuration\n", nodeNumber);
    return;
  }

  FastLED.clear(true);
}

void SolarisClass::executeCommand(uint8_t nodeNumber)
{
  bright = constrain(map(SUN.charge, 0, SOLARIS::SAFE_CHARGE,
                         SOLARIS::MIN_BRIGHT, SOLARIS::MAX_BRIGHT),
                     SOLARIS::MIN_BRIGHT, SOLARIS::MAX_BRIGHT);

  switch (SUN.command)
  {
  case 12:
    onOff(false);
    break;
  case 13:
    onOff(true);
    break;
  case 14:
    strobe(SOLARIS::INTERVAL, true);
    break;
  case 15:
    crossFade(SOLARIS::CROSS_FADE_INTERVAL, true);
    break;
  case 16:
    breathe(SOLARIS::BREATHE_INTERVAL_1, SOLARIS::BREATHE_INTERVAL_2);
    break;
  case 17:
    breatheSingle(SOLARIS::BREATHE_INTERVAL_1, SOLARIS::BREATHE_INTERVAL_2);
    break;
  case 18:
    breatheSingle(SOLARIS::BREATHE_INTERVAL_1, 40 * SUN.parameter);
    break;
  case 19:
    strobe(SUN.parameter, true);
    break;
  case 20:
    fadeParameter(255, 0, 40 * SUN.parameter);
    break;
  case 21:
    fadeParameter(0, 255, 40 * SUN.parameter);
    break;
  case 22:
    crossFade(40 * SUN.parameter, true);
    break;
  case 23:
    breatheHard(SOLARIS::BREATHE_INTERVAL_1, SUN.constantCommands[0]);
    break;
  case 24:
    fadeToStrobe(100 * SUN.parameter, SUN.constantCommands[0] * 10);
    break;
  case 25:
    variableCrossFade(50 * SUN.parameter, SUN.constantCommands[0]);
    break;
  case 26:
  {
    const uint8_t level = map(SUN.parameter, 1, 127, 0, 255);
    setBrightness(level);
    break;
  }
  case 27:
    fadeParameter(0, 32, 40 * SUN.parameter);
    break;
  case 28:
    fadeParameter(255, 32, 40 * SUN.parameter);
    break;
  case 29:
    fadeParameter(32, 0, 40 * SUN.parameter);
    break;
  case 30:
    fadeParameter(32, 255, 40 * SUN.parameter);
    break;
  case 31:
    fadeParameter(static_cast<uint8_t>(4 * (static_cast<int>(SUN.parameter) - 48)), 1, 250);
    break;
  case 32:
    fadeParameter(6, 0, 40 * SUN.parameter);
    break;
  case 40:
    analogWrite(SOLARIS::EXTRA_PWM_PIN, SUN.parameter * 2);
    break;
  default:
    strobe(SOLARIS::INTERVAL, true);
    break;
  }

  FastLED.show();
}

uint32_t SolarisClass::elapsedSinceCommand() const
{
  return SUN.getGlobalTime() - SUN.startMillis;
}

bool SolarisClass::registerStrip(CRGB *pixels, uint16_t count, uint8_t dataPin)
{
  if (pixels == nullptr || count == 0)
  {
    return false;
  }

  switch (dataPin)
  {
  case 33:
    FastLED.addLeds<WS2812B, 33, GRB>(pixels, count);
    return true;
  case 34:
    FastLED.addLeds<WS2812B, 34, GRB>(pixels, count);
    return true;
  case 35:
    FastLED.addLeds<WS2812B, 35, GRB>(pixels, count);
    return true;
  case 36:
    FastLED.addLeds<WS2812B, 36, GRB>(pixels, count);
    return true;
  default:
    return false;
  }
}

void SolarisClass::writeStrip(CRGB *pixels, uint16_t count, uint8_t brightness)
{
  fill_solid(pixels, count, CRGB(brightness, brightness, brightness));
}

void SolarisClass::strobe(uint32_t interval, bool state)
{
  interval = max(interval, 1U);
  const bool isOn = state && (elapsedSinceCommand() % (2 * interval) > interval);
  const uint8_t output = isOn ? bright : 0;

  analogWrite(SOLARIS::INNER_PWM_PIN, output);
  analogWrite(SOLARIS::MIDDLE_PWM_PIN, output);
  analogWrite(SOLARIS::OUTER_PWM_PIN, output);
  analogWrite(SOLARIS::EXTRA_PWM_PIN, handsValue(output * SOLARIS::HANDS_MAX_BRIGHT / 255));

  writeStrip(innerStrip, innerCount, output);
  writeStrip(middleStrip, middleCount, output);
  writeStrip(outerStrip, outerCount, output);
}

void SolarisClass::crossFade(uint32_t duration, bool state)
{
  if (!state)
  {
    analogWrite(SOLARIS::INNER_PWM_PIN, 0);
    analogWrite(SOLARIS::MIDDLE_PWM_PIN, 0);
    analogWrite(SOLARIS::OUTER_PWM_PIN, 0);
    analogWrite(SOLARIS::EXTRA_PWM_PIN, handsValue(0));
    writeStrip(innerStrip, innerCount, 0);
    writeStrip(middleStrip, middleCount, 0);
    writeStrip(outerStrip, outerCount, 0);
    return;
  }

  duration = max(duration, 1U);
  const uint32_t elapsed = elapsedSinceCommand() % duration;
  const float phase = 2.0f * PI * elapsed / duration;
  const uint8_t inner = 128 - 127 * cosf(phase);
  const uint8_t middle = 128 - 127 * cosf(phase + 2.0f * PI / 3.0f);
  const uint8_t outer = 128 - 127 * cosf(phase + 4.0f * PI / 3.0f);
  const uint8_t innerOutput = inner * bright / 255;
  const uint8_t middleOutput = middle * bright / 255;
  const uint8_t outerOutput = outer * bright / 255;

  analogWrite(SOLARIS::INNER_PWM_PIN, innerOutput);
  analogWrite(SOLARIS::MIDDLE_PWM_PIN, middleOutput);
  analogWrite(SOLARIS::OUTER_PWM_PIN, outerOutput);
  analogWrite(SOLARIS::EXTRA_PWM_PIN,
              handsValue(SOLARIS::HANDS_MAX_BRIGHT * bright / 255));
  writeStrip(innerStrip, innerCount, innerOutput);
  writeStrip(middleStrip, middleCount, middleOutput);
  writeStrip(outerStrip, outerCount, outerOutput);
}

void SolarisClass::variableCrossFade(uint32_t duration, uint32_t interval)
{
  const uint32_t elapsed = elapsedSinceCommand();
  uint32_t period = 1;
  if (duration > elapsed && duration != 0)
  {
    period = interval - static_cast<uint32_t>(
                              static_cast<uint64_t>(interval) * elapsed / duration);
    period = max(period, 1U);
  }
  crossFade(40U * period, true);
}

void SolarisClass::onOff(bool state)
{
  const uint8_t output = state ? bright : 0;
  const uint8_t hands = bright * SOLARIS::HANDS_MAX_BRIGHT / 255U;

  analogWrite(SOLARIS::INNER_PWM_PIN, output);
  analogWrite(SOLARIS::MIDDLE_PWM_PIN, output);
  analogWrite(SOLARIS::OUTER_PWM_PIN, output);
  analogWrite(SOLARIS::EXTRA_PWM_PIN, handsValue(state ? hands : 0));
  writeStrip(innerStrip, innerCount, output);
  writeStrip(middleStrip, middleCount, output);
  writeStrip(outerStrip, outerCount, output);
}

void SolarisClass::breathe(uint32_t inhaleDuration, uint32_t exhaleDuration)
{
  inhaleDuration = max(inhaleDuration, 1U);
  exhaleDuration = max(exhaleDuration, 1U);
  const uint32_t elapsed = elapsedSinceCommand();
  uint8_t level = 0;

  if (elapsed <= inhaleDuration)
  {
    level = 255U * elapsed / inhaleDuration;
  }
  else if (elapsed <= 2U * inhaleDuration)
  {
    level = 255U - 255U * (elapsed - inhaleDuration) / inhaleDuration;
  }
  else if (elapsed <= 3U * inhaleDuration)
  {
    level = 255U * (elapsed - 2U * inhaleDuration) / inhaleDuration;
  }
  else if (elapsed <= 3U * inhaleDuration + exhaleDuration)
  {
    level = 255U - 255U * (elapsed - 3U * inhaleDuration) / exhaleDuration;
  }

  const uint8_t output = bright * level / 255U;
  analogWrite(SOLARIS::INNER_PWM_PIN, output);
  analogWrite(SOLARIS::MIDDLE_PWM_PIN, output);
  analogWrite(SOLARIS::OUTER_PWM_PIN, output);
  analogWrite(SOLARIS::EXTRA_PWM_PIN,
              handsValue(SOLARIS::HANDS_MAX_BRIGHT * output / 255U));
  writeStrip(innerStrip, innerCount, output);
  writeStrip(middleStrip, middleCount, output);
  writeStrip(outerStrip, outerCount, output);
}

void SolarisClass::breatheSingle(uint32_t inhaleDuration, uint32_t exhaleDuration)
{
  inhaleDuration = max(inhaleDuration, 1U);
  exhaleDuration = max(exhaleDuration, 1U);
  const uint32_t elapsed = elapsedSinceCommand();
  uint8_t level = 0;

  if (elapsed <= inhaleDuration)
  {
    level = 255U * elapsed / inhaleDuration;
  }
  else if (elapsed <= inhaleDuration + exhaleDuration)
  {
    level = 255U - 255U * (elapsed - inhaleDuration) / exhaleDuration;
  }

  const uint8_t output = bright * level / 255U;
  analogWrite(SOLARIS::INNER_PWM_PIN, output);
  analogWrite(SOLARIS::MIDDLE_PWM_PIN, output);
  analogWrite(SOLARIS::OUTER_PWM_PIN, output);
  analogWrite(SOLARIS::EXTRA_PWM_PIN,
              handsValue(SOLARIS::HANDS_MAX_BRIGHT * output / 255U));
  writeStrip(innerStrip, innerCount, output);
  writeStrip(middleStrip, middleCount, output);
  writeStrip(outerStrip , outerCount, output);
}

void SolarisClass::breatheHard(uint32_t inhaleDuration, uint32_t exhaleDuration)
{
  inhaleDuration = max(inhaleDuration, 1U);
  exhaleDuration = max(exhaleDuration, 1U);
  const uint32_t elapsed = elapsedSinceCommand();
  uint8_t level = 0;

  if (elapsed <= inhaleDuration)
  {
    level = 255U * elapsed / inhaleDuration;
  }
  else if (elapsed <= inhaleDuration + exhaleDuration)
  {
    const float progress = 4.0f * (elapsed - inhaleDuration) / exhaleDuration;
    level = constrain(static_cast<int>(127.0f * (2.0f - sqrtf(progress))), 0, 255);
  }

  const uint8_t output = bright * level / 255U;
  analogWrite(SOLARIS::INNER_PWM_PIN, output);
  analogWrite(SOLARIS::MIDDLE_PWM_PIN, output);
  analogWrite(SOLARIS::OUTER_PWM_PIN, output);
  analogWrite(SOLARIS::EXTRA_PWM_PIN,
              handsValue(SOLARIS::HANDS_MAX_BRIGHT * output / 255U));
  writeStrip(innerStrip, innerCount, output);
  writeStrip(middleStrip, middleCount, output);
  writeStrip(outerStrip , outerCount, output);
}

void SolarisClass::fadeParameter(uint8_t startValue, uint8_t endValue, uint32_t duration)
{
  const uint32_t elapsed = elapsedSinceCommand();
  int32_t level = endValue;
  if (duration != 0 && elapsed < duration)
  {
    level = static_cast<int32_t>(startValue) +
            (static_cast<int32_t>(endValue) - startValue) * elapsed / duration;
  }

  const uint8_t output = bright * level / 255U;
  analogWrite(SOLARIS::INNER_PWM_PIN, output);
  analogWrite(SOLARIS::MIDDLE_PWM_PIN, output);
  analogWrite(SOLARIS::OUTER_PWM_PIN, output);
  analogWrite(SOLARIS::EXTRA_PWM_PIN,
              handsValue(SOLARIS::HANDS_MAX_BRIGHT * output / 255U));
  writeStrip(innerStrip, innerCount, output);
  writeStrip(middleStrip, middleCount, output);
  writeStrip(outerStrip , outerCount, output);
}

void SolarisClass::fadeToStrobe(uint32_t duration, uint32_t interval)
{
  const uint32_t elapsed = elapsedSinceCommand();
  uint32_t period = 0;
  if (duration > elapsed && duration != 0)
  {
    period = interval - static_cast<uint32_t>(
                             static_cast<uint64_t>(interval) * elapsed / duration);
  }
  strobe(period, true);
}

void SolarisClass::setBrightness(uint8_t brightness)
{
  const uint8_t output = brightness * bright / 255U;
  analogWrite(SOLARIS::INNER_PWM_PIN, output);
  analogWrite(SOLARIS::MIDDLE_PWM_PIN, output);
  analogWrite(SOLARIS::OUTER_PWM_PIN, output);
  analogWrite(SOLARIS::EXTRA_PWM_PIN,
              handsValue(SOLARIS::HANDS_MAX_BRIGHT * bright * brightness / 65025U));
  writeStrip(innerStrip, innerCount, output);
  writeStrip(middleStrip, middleCount, output);
  writeStrip(outerStrip , outerCount, output);
}

uint8_t SolarisClass::handsValue(uint8_t input) const
{
  return 255U - input;
}

SolarisClass Solaris;