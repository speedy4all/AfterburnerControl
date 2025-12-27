#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include <Arduino.h>
#include <FastLED.h>
#include "settings.h"
#include "constants.h"

class LEDEffects {
private:
  CRGB* leds;
  uint16_t numLeds;  // LEDs per ring (total LEDs = numLeds * 2 for dual turbines)
  unsigned long lastUpdate;
  uint8_t noiseOffset;
  
  // Afterburner colors
  CRGB abCoreColor1;  // Violet-blue
  CRGB abCoreColor2;  // Magenta-purple
  
public:
  LEDEffects();
  ~LEDEffects();
  void begin(uint16_t totalLedCount);  // Total LEDs (numLeds * 2 for dual turbines)
  void update(uint16_t newTotalLedCount);
  void render(const AfterburnerSettings& settings, float throttle);
  void setBrightness(uint8_t brightness);
  
private:
  // Helper methods for dual-ring support
  bool isRing2(uint16_t ledIndex) const;
  uint16_t getRingLocalIndex(uint16_t ledIndex) const;
  float getRingPosition(uint16_t ledIndex) const;
  
  void renderCoreEffect(const AfterburnerSettings& settings, float throttle);
  void renderAfterburnerOverlay(const AfterburnerSettings& settings, float throttle);
  float getEasedThrottle(float throttle, uint8_t mode);
  void addFlicker(uint16_t ledIndex, uint8_t intensity, const AfterburnerSettings& settings);
  void addSparkles(float abIntensity, const AfterburnerSettings& settings);
  CRGB lerpColor(CRGB color1, CRGB color2, float factor);
};

#endif // LED_EFFECTS_H
