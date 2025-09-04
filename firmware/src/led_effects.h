#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include <Arduino.h>
#include <FastLED.h>
#include "settings.h"
#include "constants.h"

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
  void addFlicker(uint16_t ledIndex, uint8_t intensity, const AfterburnerSettings& settings);
  void addSparkles(float abIntensity, const AfterburnerSettings& settings);
  CRGB lerpColor(CRGB color1, CRGB color2, float factor);
};

#endif // LED_EFFECTS_H
