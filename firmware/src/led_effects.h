#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include <FastLED.h>
#include "settings.h"

// Pin definitions for ESP8266 NodeMCU V3
#define LED_PIN 12  // D6 - GPIO12

class LEDEffects {
private:
  CRGB* leds;
  uint16_t numLeds;
  unsigned long lastUpdate;
  uint8_t noiseOffset;
  
  // Afterburner colors
  CRGB abCoreColor1;  // Violet-blue
  CRGB abCoreColor2;  // Magenta-purple
  
public:
  LEDEffects();
  ~LEDEffects();
  void begin(uint16_t ledCount);
  void update(uint16_t newLedCount);
  void render(const AfterburnerSettings& settings, float throttle);
  void setBrightness(uint8_t brightness);
  
private:
  void renderCoreEffect(const AfterburnerSettings& settings, float throttle);
  void renderAfterburnerOverlay(const AfterburnerSettings& settings, float throttle);
  float getEasedThrottle(float throttle, uint8_t mode);
  void addFlicker(uint16_t ledIndex, uint8_t intensity);
  void addSparkles(float abIntensity);
  CRGB lerpColor(CRGB color1, CRGB color2, float factor);
};

#endif // LED_EFFECTS_H
