#include "led_effects.h"
#include <math.h>

/*
 * Speed Setting Usage:
 * The speedMs setting (100-5000ms) controls animation timing for various effects:
 * 
 * 1. Pulse Mode (Mode 2): Controls how fast the afterburner pulses
 *    - 100ms = Very fast pulse (10Hz)
 *    - 1200ms = Medium pulse (0.83Hz) 
 *    - 5000ms = Slow pulse (0.2Hz)
 * 
 * 2. Breathing Effect (Modes 1 & 2): Controls breathing animation speed
 *    - 100ms = Rapid breathing
 *    - 1200ms = Normal breathing
 *    - 5000ms = Slow breathing
 * 
 * 3. Flicker Effect: Controls how fast the flicker animation cycles
 *    - 100ms = Fast flicker
 *    - 1200ms = Normal flicker
 *    - 5000ms = Slow flicker
 * 
 * 4. Sparkle Effect: Controls sparkle frequency during afterburner
 *    - 100ms = Many sparkles
 *    - 1200ms = Normal sparkles
 *    - 5000ms = Few sparkles
 */

// Fallback for M_PI if not defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

LEDEffects::LEDEffects() {
  leds = nullptr;
  numLeds = 0;
  lastUpdate = 0;
  noiseOffset = 0;
  
  // Initialize afterburner colors
  abCoreColor1 = CRGB(90, 60, 255);   // Violet-blue
  abCoreColor2 = CRGB(255, 90, 255);  // Magenta-purple
}

LEDEffects::~LEDEffects() {
  if (leds) {
    delete[] leds;
  }
}

void LEDEffects::begin(uint16_t ledCount) {
  if (leds) {
    delete[] leds;
  }
  
  numLeds = ledCount;
  leds = new CRGB[numLeds];
  
  FastLED.addLeds<WS2812B, LED_STRIP_PIN, GRB>(leds, numLeds);
  FastLED.setBrightness(200);
  FastLED.clear();
  FastLED.show();
}

void LEDEffects::update(uint16_t newLedCount) {
  if (newLedCount != numLeds) {
    begin(newLedCount);
  }
}

void LEDEffects::render(const AfterburnerSettings& settings, float throttle) {
  // Clear all LEDs
  FastLED.clear();
  
  // Render core effect
  renderCoreEffect(settings, throttle);
  
  // Render afterburner overlay
  renderAfterburnerOverlay(settings, throttle);
  
  // Update brightness
  FastLED.setBrightness(settings.brightness);
  
  // Show the LEDs
  FastLED.show();
  
  // Update noise offset for flicker
  noiseOffset++;
}

void LEDEffects::setBrightness(uint8_t brightness) {
  FastLED.setBrightness(brightness);
}

void LEDEffects::renderCoreEffect(const AfterburnerSettings& settings, float throttle) {
  // Get eased throttle value based on mode
  float easedThrottle = getEasedThrottle(throttle, settings.mode);
  
  // Create start and end colors
  CRGB startColor = CRGB(settings.startColor[0], settings.startColor[1], settings.startColor[2]);
  CRGB endColor = CRGB(settings.endColor[0], settings.endColor[1], settings.endColor[2]);
  
  // Calculate base brightness proportional to throttle
  uint8_t baseBrightness = 30 + (uint8_t)(200 * throttle);
  
  // Add breathing effect based on speed setting (for Ease and Pulse modes)
  if (settings.mode == 1 || settings.mode == 2) {
    // Use speedMs to control breathing frequency
    float breathingSpeed = 1000.0f / (float)settings.speedMs;
    float breathing = 0.8f + 0.2f * sin(millis() * breathingSpeed);
    baseBrightness = (uint8_t)(baseBrightness * breathing);
  }
  
  // Render each LED
  for (uint16_t i = 0; i < numLeds; i++) {
    // Interpolate color based on eased throttle
    CRGB color = lerpColor(startColor, endColor, easedThrottle);
    
    // Apply base brightness
    color.nscale8(baseBrightness);
    
    // Add flicker effect
    addFlicker(i, 20, settings);
    
    // Set the LED
    leds[i] = color;
  }
}

void LEDEffects::renderAfterburnerOverlay(const AfterburnerSettings& settings, float throttle) {
  // Calculate afterburner threshold
  float abThreshold = settings.abThreshold / 100.0f;
  
  if (throttle <= abThreshold) {
    return; // No afterburner effect
  }
  
  // Calculate afterburner intensity
  float abIntensity = (throttle - abThreshold) / (1.0f - abThreshold);
  abIntensity = constrain(abIntensity, 0.0f, 1.0f);
  
  // Apply pulse modulation for Pulse mode
  if (settings.mode == 2) {
    // Use speedMs to control pulse frequency (faster speed = faster pulse)
    // Convert speedMs to frequency: 100ms = fast pulse, 5000ms = slow pulse
    float pulseFrequency = 1000.0f / (float)settings.speedMs;
    float pulse = 0.6f + 0.4f * sin(millis() * pulseFrequency);
    abIntensity *= pulse;
  }
  
  // Render afterburner effect
  for (uint16_t i = 0; i < numLeds; i++) {
    // Calculate spatial profile (stronger in center)
    float position = (float)i / numLeds;
    float spatialProfile = 0.65f + 0.35f * sin(2.0f * M_PI * position);
    
    // Blend afterburner colors based on throttle
    CRGB abColor = lerpColor(abCoreColor1, abCoreColor2, throttle);
    
    // Scale by intensity and spatial profile
    uint8_t abBrightness = (uint8_t)(255 * abIntensity * spatialProfile);
    abColor.nscale8(abBrightness);
    
    // Add to existing LED color
    leds[i] += abColor;
  }
  
  // Add sparkles when afterburner is strong
  if (abIntensity > 0.4f) {
    addSparkles(abIntensity, settings);
  }
}

float LEDEffects::getEasedThrottle(float throttle, uint8_t mode) {
  switch (mode) {
    case 0: // Linear
      return throttle;
    case 1: // Ease
      return pow(throttle, 1.2f);
    case 2: // Pulse
      return pow(throttle, 1.2f);
    default:
      return throttle;
  }
}

void LEDEffects::addFlicker(uint16_t ledIndex, uint8_t intensity, const AfterburnerSettings& settings) {
  // Generate noise-based flicker using FastLED noise functions
  // Use speedMs to control flicker speed (faster speed = faster flicker)
  float flickerSpeed = 1000.0f / (float)settings.speedMs;
  uint8_t noise = inoise8(ledIndex * 12, (millis() * flickerSpeed + ledIndex * 7) * 8 + noiseOffset);
  
  // Map noise to flicker range
  int8_t flicker = map(noise, 0, 255, -intensity, intensity);
  
  // Apply flicker to LED
  leds[ledIndex].addToRGB(flicker);
}

void LEDEffects::addSparkles(float abIntensity, const AfterburnerSettings& settings) {
  // Add random white sparkles
  // Use speedMs to control sparkle frequency (faster speed = more sparkles)
  float sparkleFrequency = 1000.0f / (float)settings.speedMs;
  uint16_t sparkleChance = (uint16_t)(abIntensity * 50 * sparkleFrequency);
  
  for (uint16_t i = 0; i < numLeds; i++) {
    if (random(1000) < sparkleChance) {
      uint8_t sparkleIntensity = random(50, 150);
      leds[i] += CRGB(sparkleIntensity, sparkleIntensity, sparkleIntensity);
    }
  }
}

CRGB LEDEffects::lerpColor(CRGB color1, CRGB color2, float factor) {
  factor = constrain(factor, 0.0f, 1.0f);
  
  CRGB result;
  result.r = (uint8_t)(color1.r + (color2.r - color1.r) * factor);
  result.g = (uint8_t)(color1.g + (color2.g - color1.g) * factor);
  result.b = (uint8_t)(color1.b + (color2.b - color1.b) * factor);
  
  return result;
}
