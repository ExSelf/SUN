#include <Arduino.h>
#include "Solaris.h"


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

void SolarisClass::setupNode(uint8_t nodeNumber)
{
  pinMode(SOLARIS::INNER_PWM_PIN, OUTPUT);
  pinMode(SOLARIS::MIDDLE_PWM_PIN, OUTPUT);
  pinMode(SOLARIS::OUTER_PWM_PIN, OUTPUT);
  pinMode(SOLARIS::EXTRA_PWM_PIN, OUTPUT);
}

void SolarisClass::executeCommand(uint8_t nodeNumber)
{
  switch (SUN.command) {

    /////////////////////////////// MIDI ///////////////////////////////

    // OFF
    case 12:
      OnOff(false);
      break;

    // ON
    case 13:
      OnOff(true);
      break;

    // Strobe
    case 14:
      Strobe(interval, true);
      break;

    // Cross-Fade
    case 15:
      CrossFade(sinInterval, true);
      break;

    // 2 Breathes
    case 16:
      Breathe(breatheint1, breatheint2);
      break;

    // 1 breathe
    case 17:
      BreatheSingle(breatheint1, breatheint2);
      break;

    // Breathe (parameter based)
    case 18:
      BreatheSingle(breatheint1, 40 * SUN.parameter);
      break;

    // Strobe (parameter based)
    case 19:
      Strobe(SUN.parameter, true);
      break;

    // FadeOut (parameter based)
    case 20:
      FadeParameter(255, 0, 40 * SUN.parameter);
      break;

    // FadeIn (parameter based)
    case 21:
      FadeParameter(0, 255, 40 * SUN.parameter);
      break;

    // Cross-Fade (parameter based)
    case 22:
      CrossFade(40 * SUN.parameter, true);
      break;

    // Complicated breathe (parameter based)
    case 23:
      BreatheHard(breatheint1, constant_commands[0]);
      break;

    // Expanding Strobe (parameter based)
    case 24:
      FadeToStrobe(100 * SUN.parameter, constant_commands[0] * 10);
      break;

    // Accelerating Cross-Fade
    case 25:
      VariableCrossFade(50 * SUN.parameter, constant_commands[0]);
      break;

    // Brightness (parameter based)
    case 26:
      {
        byte b = map(SUN.parameter, 1, 127, 0, 255);
        Brightness(b);
        break;
      }

    // Fade from 0 to 32 (parameter based)
    case 27:
      FadeParameter(0, 32, 40 * SUN.parameter);
      break;

    // Fade from 255 to 32 (parameter based)
    case 28:
      FadeParameter(255, 32, 40 * SUN.parameter);
      break;

    // Fade from 32 to 0 (parameter based)
    case 29:
      FadeParameter(32, 0, 40 * SUN.parameter);
      break;

    // Fade from 32 to 255 (parameter based)
    case 30:
      FadeParameter(32, 255, 40 * SUN.parameter);
      break;

    case 31:
      FadeParameter(4 * (SUN.parameter - 48), 1, 250);
      break;

    // Fade from 32 to 0 (parameter based)
    case 32:
      FadeParameter(6, 0, 40 * SUN.parameter);
      break;

    case 40:
      analogWrite(SOLARIS::EXTRA_PWM_PIN, SUN.parameter * 2);
      break;

    default:
      Strobe(interval, true);
      break;
  }

  inner.show();
  middle.show();
  outer.show();

}

SolarisClass Solaris;