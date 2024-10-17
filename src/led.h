#include <Arduino.h>
#include <FastLED.h>
#include "config.h"
#include "settings.h"
#include "sound.h"

// ===== ДЛЯ РАЗРАБОВ =====
const int NUM_LEDS = (EDGE_LEDS * 24);
CRGBPalette16 currentPalette, linearPalette;
const int FACE_SIZE = EDGE_LEDS * 4;
const int LINE_SIZE = EDGE_LEDS;
#define PAL_STEP 30
CRGB leds[NUM_LEDS];

int perlinPoint;
int curBright = 50;
bool fadeFlag = false;
bool mode = true;
bool colorMode = true;
uint16_t counter = 0;
byte speed = 15;
uint32_t tmrDraw, tmrColor, tmrFade;

TaskHandle_t ledEffectsTask;

// масштабируем шум
byte getMaxNoise(uint16_t x, uint16_t y) {
  return constrain(map(inoise8(x, y), 50, 200, 0, 255), 0, 255);
}

uint32_t getPixColor(CRGB color) {
  return (((uint32_t)color.r << 16) | (color.g << 8) | color.b);
}

// заливка всех 6 граней в одинаковом порядке
void fillSimple(int num, CRGB color) {  // num 0-NUM_LEDS / 6
  for (byte i = 0; i < 6; i++) {
    leds[i * FACE_SIZE + num] = color;
  }
}

void changeMode() {
  if (!random(3)) mode = !mode;
  speed = random(4, 12);
  colorMode = !colorMode;
  int thisDebth = random(0, 32768);
  byte thisStep = random(2, 7) * 5;
  bool sparkles = !random(5);

  if (colorMode) {
    for (int i = 0; i < 16; i++) {
      currentPalette[i] = CHSV(getMaxNoise(thisDebth + i * thisStep, thisDebth),
                               sparkles ? (!random(9) ? 30 : 255) : 255,
                               constrain((i + 7) * (i + 7), 0, 255));
    }
  } else {
    for (int i = 0; i < 4; i++) {
      CHSV color = CHSV(random(0, 256), random(0, 256), (uint8_t)(i + 1) * 64 - 1);
      for (byte j = 0; j < 4; j++) {
        currentPalette[i * 4 + j] = color;
      }
    }
  }
}

// заливка из четырёх вершин
void fillVertex(int num, CRGB color) { // num 0-NUM_LEDS / 6
  num /= 4;
  byte thisRow = 0;
  for (byte i = 0; i < 3; i++) {
    leds[LINE_SIZE * thisRow + num] = color;
    thisRow += 2;
    leds[LINE_SIZE * thisRow - num - 1] = color;
    leds[LINE_SIZE * thisRow + num] = color;
    thisRow += 2;
    leds[LINE_SIZE * thisRow - num - 1] = color;
  }
  thisRow = 13;
  for (byte i = 0; i < 3; i++) {
    leds[LINE_SIZE * thisRow - num - 1] = color;
    leds[LINE_SIZE * thisRow + num] = color;
    thisRow += 2;
    leds[LINE_SIZE * thisRow - num - 1] = color;
    leds[LINE_SIZE * thisRow + num] = color;
    thisRow += 2;
  }
}

// рандом сид из сырого аналога
uint32_t getEntropy(byte pin) {
  unsigned long seed = 0;
  for (int i = 0; i < 400; i++) {
    seed = 1;
    for (byte j = 0; j < 16; j++) {
      seed *= 4;
      seed += analogRead(pin) & 3;
    }
  }
  return seed;
}

void toggleLeds() {
  settings.ledEnable = ! settings.ledEnable;
  if (settings.ledEnable == false) {
    FastLED.clear();
    FastLED.show();
  }

  Serial.print("LED: ");
  Serial.println(settings.ledEnable ? "On" : "Off");
}

void ledEffects( void * parameter) {
  for(;;) {
    if (/* settings.ledEnable */ true) {
      if (millis() - tmrDraw >= 20) {
        tmrDraw = millis();

        if (settings.soundMode == true) {
          FastLED.clear();
          int thisSound = getSoundLength();

          int thisL = (float)FACE_SIZE * thisSound / 100.0 + 1;
          thisL = constrain(thisL, 0, FACE_SIZE);
          for (int i = 0; i < thisL; i++) {
           fillVertex(i, ColorFromPalette(currentPalette, (float)255 * i / FACE_SIZE / 2 + counter / 4, 255, LINEARBLEND));
           // fillColumns(i, ColorFromPalette(currentPalette, (float)255 * i / FACE_SIZE / 2 + counter / 10, 255, LINEARBLEND));
          }

        } else {
          for (int i = 0; i < FACE_SIZE; i++) {
            if (mode) fillSimple(i, ColorFromPalette(currentPalette, getMaxNoise(i * PAL_STEP + counter, counter), 255, LINEARBLEND));
            else fillVertex(i, ColorFromPalette(currentPalette, getMaxNoise(i * PAL_STEP / 4 + counter, counter), 255, LINEARBLEND));
          }
        }

        FastLED.show();
        counter += speed;
      }

      // смена режима и цвета
      if (millis() - tmrColor >= CHANGE_PRD * 1000L) {
        tmrColor = millis();
        fadeFlag = true;
      }

      // фейдер для смены через чёрный
      if (fadeFlag && millis() - tmrFade >= 30) {
        static int8_t fadeDir = -1;
        tmrFade = millis();
        if (fadeFlag) {
          curBright += 5 * fadeDir;

          if (curBright < 5) {
            curBright = 5;
            fadeDir = 1;
            changeMode();
          }
          if (curBright > settings.bright) {
            curBright = settings.bright;
            fadeDir = -1;
            fadeFlag = false;
          }
          FastLED.setBrightness(curBright);
        }
      }
    }
  }
}

void ledInitOnCore(const BaseType_t xCoreID) {
  FastLED.addLeds<WS2812, LED_DI, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  if (CUR_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CUR_LIMIT);
  FastLED.setBrightness(settings.bright);
  curBright = settings.bright;
  FastLED.clear();

  randomSeed(getEntropy(GPIO_NUM_36));   // My system's in decline, EMBRACING ENTROPY!
  perlinPoint = random(0, 32768);
  fadeFlag = true;  // сразу флаг на смену режима

  xTaskCreatePinnedToCore(
        ledEffects, /* Function to implement the task */
        "led_effects", /* Name of the task */
        10000,  /* Stack size in words */
        NULL,  /* Task input parameter */
        0,  /* Priority of the task */
        &ledEffectsTask,  /* Task handle. */
        0
  );
}