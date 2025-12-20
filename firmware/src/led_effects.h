#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include <Arduino.h>
#include "settings.h"
#include "constants.h"
#include "led_driver.h"

class LEDEffects {
private:
  LEDDriver* ledDriver;
  unsigned long lastUpdate;
  uint8_t noiseOffset;

public:
  LEDEffects();
  ~LEDEffects();
  void begin(LEDDriver* driver);
  void render(const AfterburnerSettings& settings, float throttle);
  void setBrightness(uint8_t brightness);
  
private:
  void renderCoreEffect(const AfterburnerSettings& settings, float throttle, float channelIntensities[4]);
  void renderAfterburnerOverlay(const AfterburnerSettings& settings, float throttle, float channelIntensities[4]);
  float getEasedThrottle(float throttle, uint8_t mode);
  void addFlicker(float channelIntensities[4], const AfterburnerSettings& settings);
  void addSparkles(float channelIntensities[4], float abIntensity, const AfterburnerSettings& settings);
  float calculateIntensityFromColor(uint8_t color[3]);
};

#endif // LED_EFFECTS_H
