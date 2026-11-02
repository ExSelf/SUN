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

void Strobe(int i, boolean state) {
  if (((mil + mesh_millis_offset - start_millis) % (2 * i) > i) && state) {
    ledState = true;
  } else {
    ledState = false;
  }

  analogWrite(INNER_PWM_PIN, ledState * bright);
  analogWrite(MIDDLE_PWM_PIN, ledState * bright);
  analogWrite(OUTER_PWM_PIN, ledState * bright);
  analogWrite(HANDS_PWM_PIN, HandsValue(ledState * bright * HANDS_MAX_BRIGHT / 255));
}

void CrossFade(int sinInterval, boolean state) {
  if (state) {
    byte bright1 = 128 - 127 * cos(2 * PI / sinInterval * (mil + mesh_millis_offset - start_millis));
    byte bright2 = 128 - 127 * cos(2 * PI / sinInterval * ((mil + mesh_millis_offset - start_millis) + (sinInterval / 3)));
    byte bright3 = 128 - 127 * cos(2 * PI / sinInterval * ((mil + mesh_millis_offset - start_millis) + (2 * sinInterval / 3)));

    if (debug) Serial.println(bright1);

    analogWrite(INNER_PWM_PIN, bright1 * bright / 255);
    pixelStripWrite(inner, bright1 * bright / 255);

    analogWrite(MIDDLE_PWM_PIN, bright2 * bright / 255);
    pixelStripWrite(middle, bright2 * bright / 255);

    analogWrite(OUTER_PWM_PIN, bright3 * bright / 255);
    pixelStripWrite(outer, bright3 * bright / 255);

    analogWrite(HANDS_PWM_PIN, HandsValue(HANDS_MAX_BRIGHT * bright / 255));

  } else {
    analogWrite(INNER_PWM_PIN, 0);
    analogWrite(MIDDLE_PWM_PIN, 0);
    analogWrite(OUTER_PWM_PIN, 0);
    analogWrite(HANDS_PWM_PIN, HandsValue(0));
  }
}



void VariableCrossFade(int t, unsigned long interval) {
  int p;
  if (start_millis + t > (mil + mesh_millis_offset)) {
    p = interval - interval * (mil + mesh_millis_offset - start_millis) / t;
  } else {
    p = 1;
  }

  CrossFade(40 * p, true);
}


void OnOff(boolean state) {

  byte h = bright * HANDS_MAX_BRIGHT / 255;

  analogWrite(INNER_PWM_PIN, bright * state);
  analogWrite(MIDDLE_PWM_PIN, bright * state);
  analogWrite(OUTER_PWM_PIN, bright * state);
  analogWrite(HANDS_PWM_PIN, HandsValue(h * state));

  pixelStripWrite(inner, bright * state);
  pixelStripWrite(middle, bright * state);
  pixelStripWrite(outer, bright * state);
}

void Breathe(int a, int b) {
  byte breathe_bright;

  long t = (mil + mesh_millis_offset - start_millis);

  if (t <= a) {  // первый вздох
    breathe_bright = 255 * t / a;
  } else if (t <= (2 * a)) {  // первый выдох
    breathe_bright = 255 - (t - a) * 255 / a;
  } else if (t <= (3 * a)) {  // второй вздох
    breathe_bright = 255 * (t - (2 * a)) / a;
  } else if (t <= ((3 * a) + b)) {  // второй длинный выдох
    breathe_bright = 255 - (t - (3 * a)) * 255 / b;
  } else {  // после второго выдоха
    breathe_bright = 0;
    lastprogram = 0;
  }

  analogWrite(INNER_PWM_PIN, bright * breathe_bright / 255);
  analogWrite(MIDDLE_PWM_PIN, bright * breathe_bright / 255);
  analogWrite(OUTER_PWM_PIN, bright * breathe_bright / 255);
  analogWrite(HANDS_PWM_PIN, HandsValue(HANDS_MAX_BRIGHT * bright * breathe_bright / 65025));

  pixelStripWrite(inner, bright * breathe_bright / 255);
  pixelStripWrite(middle, bright * breathe_bright / 255);
  pixelStripWrite(outer, bright * breathe_bright / 255);
}

void BreatheSingle(int a, int b) {
  byte breathe_bright;

  long br = mil + mesh_millis_offset - start_millis;

  if (br <= a) {  // Вдох
    breathe_bright = 255 * br / a;
  } else if (br <= (a + b)) {  // Выдох
    breathe_bright = 255 - (br - a) * 255 / b;
  } else {  // После выдоха
    breathe_bright = 0;
  }

  analogWrite(INNER_PWM_PIN, bright * breathe_bright / 255);
  analogWrite(MIDDLE_PWM_PIN, bright * breathe_bright / 255);
  analogWrite(OUTER_PWM_PIN, bright * breathe_bright / 255);
  analogWrite(HANDS_PWM_PIN, HandsValue(HANDS_MAX_BRIGHT * bright * breathe_bright / 65025));

  pixelStripWrite(inner, bright * breathe_bright / 255);
  pixelStripWrite(middle, bright * breathe_bright / 255);
  pixelStripWrite(outer, bright * breathe_bright / 255);
}

void BreatheHard(int a, int b) {
  byte breathe_bright;

  long br_h = mil + mesh_millis_offset - start_millis;
  if (br_h <= a) {  // Вдох
    breathe_bright = 255 * br_h / a;
    //    if (debug) Serial.println(bright);
  } else if (br_h <= (a + b)) {  // Конец выдоха (1/(x+1-1/t)-t/(t-1))
    float s = 4 * (br_h - a) / b;
    breathe_bright = 127 * (2 - sqrt(s));
    if (debug) {
      Serial.print(br_h - a);
      Serial.print(" - ");
      Serial.print(s);
      Serial.print(" - ");
      Serial.println(bright);
    }
  } else {  // После выдоха
    breathe_bright = 0;
  }

  analogWrite(INNER_PWM_PIN, bright * breathe_bright / 255);
  analogWrite(MIDDLE_PWM_PIN, bright * breathe_bright / 255);
  analogWrite(OUTER_PWM_PIN, bright * breathe_bright / 255);
  analogWrite(HANDS_PWM_PIN, HandsValue(HANDS_MAX_BRIGHT * bright * breathe_bright / 65025));

  pixelStripWrite(inner, bright * breathe_bright / 255);
  pixelStripWrite(middle, bright * breathe_bright / 255);
  pixelStripWrite(outer, bright * breathe_bright / 255);
}

void FadeParameter(byte startvalue, byte endvalue, int t) {
  byte fade_bright;

  unsigned long CurrentMillis = millis();
  if (start_millis + t > CurrentMillis) {
    if (startvalue < endvalue) {
      fade_bright = startvalue + (endvalue - startvalue) * (CurrentMillis - start_millis) / t;
    } else {
      fade_bright = startvalue - (startvalue - endvalue) * (CurrentMillis - start_millis) / t;
    }
  } else {
    fade_bright = endvalue;
  }

  analogWrite(INNER_PWM_PIN, bright * fade_bright / 255);
  analogWrite(MIDDLE_PWM_PIN, bright * fade_bright / 255);
  analogWrite(OUTER_PWM_PIN, bright * fade_bright / 255);
  analogWrite(HANDS_PWM_PIN, HandsValue(HANDS_MAX_BRIGHT * bright * fade_bright / 65025));

  pixelStripWrite(inner, bright * fade_bright / 255);
  pixelStripWrite(middle, bright * fade_bright / 255);
  pixelStripWrite(outer, bright * fade_bright / 255);
}

void FadeToStrobe(int t, unsigned long interval) {
  int p;
  unsigned long CurrentMillis = millis();
  if (start_millis + t > CurrentMillis) {
    p = interval - interval * (CurrentMillis - start_millis) / t;
  } else {
    p = 0;
  }

  Strobe(p, true);
}

void Brightness(byte brightness) {
  analogWrite(INNER_PWM_PIN, brightness * bright / 255);
  analogWrite(MIDDLE_PWM_PIN, brightness * bright / 255);
  analogWrite(OUTER_PWM_PIN, brightness * bright / 255);
  analogWrite(HANDS_PWM_PIN, HandsValue(HANDS_MAX_BRIGHT * bright * brightness / 65025));

  pixelStripWrite(inner, brightness * bright / 255);
  pixelStripWrite(middle, brightness * bright / 255);
  pixelStripWrite(outer, brightness * bright / 255);
}

void pixelStripWrite(Adafruit_NeoPixel &strip, uint8_t brightness) {
  strip.fill(strip.Color(brightness, brightness, brightness), 0, strip.numPixels());
}

uint8_t HandsValue(uint8_t input) {
  uint8_t output = map(input, 0, 255, 255, 0);
  return output;
}

SolarisClass Solaris;