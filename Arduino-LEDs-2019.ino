#include <FastLED.h>

#define NUM_REG     32
#define LED_NUM     60
#define LED_PIN     8
#define LED_TYPE    WS2812B
#define BRIGHTNESS  64
#define COLOR_ORDER GRB

namespace reg {
  char registers[LED_NUM] = {
    255, 0, 0, 0, 255, 0,
    20, 0,
    0, 0, 255, 0, 0, 255,
    4, 12,
    255, 255, 255, 255, 0, 255,
    20, 0,
    0, 255, 255, 0, 0, 255,
    20, 0,
  };
  /*
    color = (3) [r, g, b]
    colorGradient = (6) [colorTop, colorBottom]
    position = (1) byte / 4
    side = (15 -> 16) [
      frontGradient,
      frontWaveSize,
      0,
      backGradient,
      backWaveSize,
      waveOffset,
    ]
    registers = (32) [
      leftSide,
      rightSide,
    ]
  */
  int index = 0;
  void update() {
    while (Serial.available()) {
      registers[index] = Serial.read();
      index++;
      index %= NUM_REG;
    }
  }
  CRGB color(int i) {
    return CRGB(registers[i], registers[i + 1], registers[i + 2]);
  }
  float position(int i) {
    return registers[i] / 4.0;
  }
}

namespace color {
  CRGB blend(CRGB a, CRGB b, float amount) {
    return CRGB(
      a.r * (1.0 - amount) + b.r * amount,
      a.g * (1.0 - amount) + b.g * amount,
      a.b * (1.0 - amount) + b.b * amount);
  }
  CRGB square(CRGB c) {
    return CRGB(
      c.r * c.r / 256,
      c.g * c.g / 256,
      c.b * c.b / 256);
  }
}

CRGB leds[LED_NUM];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, LED_NUM).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(9600);
  while (!Serial) {}

  pinMode(13, OUTPUT);
}

void wave(float *out, float frontSize, float backSize, float offset) {
  float at = offset;
  float size = frontSize + backSize;
  while (at < 0) at += size;
  for (int i = 0; i < 30; i++) {
    at++;
    while (at >= size) at -= size;
    out[i] = at < frontSize ?
      at < 1.0 ? at : 1.0 :
      at - frontSize < 1.0 ? 1.0 - (at - frontSize) : 0.0;
  }
}

void setSide(int sideId) {
  int reg = sideId * 16;
  CRGB frontTop = reg::color(reg);
  CRGB frontBottom = reg::color(reg + 3);
  CRGB backTop = reg::color(reg + 8);
  CRGB backBottom = reg::color(reg + 8 + 3);

  float frontSize = reg::position(reg + 6);
  float backSize = reg::position(reg + 8 + 6);
  float offset = reg::position(reg + 8 + 7);

  if (frontSize == 0.0 && backSize == 0.0) {
    for (int i = 0; i < 30; i++) {
      leds[sideId * 30 + i] = CRGB::Black;
    }
  } else if (frontSize == 0.0) {
    for (int i = 0; i < 30; i++) {
      CRGB back = color::blend(backTop, backBottom, i / 30.0);
      leds[sideId * 30 + i] = color::square(back);
    }
  } else if (backSize == 0.0) {
    for (int i = 0; i < 30; i++) {
      CRGB front = color::blend(frontTop, frontBottom, i / 30.0);
      leds[sideId * 30 + i] = color::square(front);
    }
  } else {
    float waveScales[30];
    wave(waveScales, backSize, frontSize, offset);
    for (int i = 0; i < 30; i++) {
      CRGB front = color::blend(frontTop, frontBottom, i / 30.0);
      CRGB back = color::blend(backTop, backBottom, i / 30.0);
      leds[sideId * 30 + i] = color::square(color::blend(front, back, waveScales[i]));
    }
  }
}

void loop() {
  reg::update();
  
  setSide(0);
  setSide(1);
  FastLED.show();
  delay(10);
}
