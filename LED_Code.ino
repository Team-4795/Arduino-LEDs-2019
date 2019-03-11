#include <FastLED.h>

#define LED_PIN     8
#define NUM_LEDS    60
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define FLASH_DELAY 100
CRGB leds[NUM_LEDS];

char toggle;

TBlendType currentBlending;

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  pinMode(13, OUTPUT);
}

void loop() {
  char color = checkCommand();
  for (int i = 0; i < 5; i++) {
    leds[i] = CRGB::White;
    leds[NUM_LEDS - 1 - i] = CRGB::White;
  }
  FastLED.show();
  toggle = color;
  flashColor(toggle);
  delay(10);
}

char checkCommand() {
  char input = Serial.read();
  if (Serial.available()) {
    if (input == 'r') {
      return 'r';
    } else if (input == 'g') {
      return 'g';
    } else if (input == 'b') {
      return 'b';
    } else if (input == 'y') {
      return 'y';
    } else {
      return 'z';
    }
  }
}

void flashColor(char color) {
  switch (color) {
    case 'r':
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Red;
      }
      FastLED.show();
      delay(FLASH_DELAY);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      FastLED.show();
      delay(FLASH_DELAY);
      break;
    case 'b':
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Blue;
      }
      FastLED.show();
      delay(FLASH_DELAY);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      FastLED.show();
      delay(FLASH_DELAY);
      break;
    case 'g':
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Green;
      }
      FastLED.show();
      delay(FLASH_DELAY);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      FastLED.show();
      delay(FLASH_DELAY);
      break;
    case 'y':
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Yellow;
      }
      FastLED.show();
      delay(FLASH_DELAY);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      FastLED.show();
      delay(FLASH_DELAY);
      break;
    case 'z':
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
  }
}

/* 
 *  Colors:
 *  r Red
 *  b Blue
 *  g Green
 *  y Yellow
 */
