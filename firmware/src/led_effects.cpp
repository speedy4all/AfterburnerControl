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
  ledDriver = nullptr;
  lastUpdate = 0;
  noiseOffset = 0;
}

LEDEffects::~LEDEffects() {
  // LEDDriver is managed externally, no cleanup needed here
}

void LEDEffects::begin(LEDDriver* driver) {
  if (!driver) {
    Serial.println("LEDEffects: ERROR - LEDDriver pointer is null!");
    return;
  }
  
  if (!driver->isInitialized()) {
    Serial.println("LEDEffects: ERROR - LEDDriver not initialized! Call ledDriver.begin() first.");
    return;
  }
  
  ledDriver = driver;
  Serial.println("LEDEffects: Initialized for 4-channel MOSFET control");
}

void LEDEffects::render(const AfterburnerSettings& settings, float throttle) {
  // All 4 channels active with base intensity proportional to throttle
  float channelIntensities[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  
  // Render core effect (base intensity for all channels)
  renderCoreEffect(settings, throttle, channelIntensities);
  
  // Render afterburner overlay (adds intensity boost with rotation)
  renderAfterburnerOverlay(settings, throttle, channelIntensities);
  
  // Apply flicker effect
  addFlicker(channelIntensities, settings);
  
  // Apply sparkles during strong afterburner
  float abThreshold = settings.abThreshold / 100.0f;
  if (throttle > abThreshold) {
    float abIntensity = (throttle - abThreshold) / (1.0f - abThreshold);
    if (abIntensity > 0.4f) {
      addSparkles(channelIntensities, abIntensity, settings);
    }
  }
  
  // Check if LEDDriver is available
  if (!ledDriver) {
    Serial.println("LEDEffects: ERROR - LEDDriver not initialized!");
    return;
  }
  
  // Convert to PWM values and apply to LEDDriver
  for (uint8_t i = 0; i < NUM_MOSFET_CHANNELS; i++) {
    // Clamp intensity to 0-1 range
    channelIntensities[i] = constrain(channelIntensities[i], 0.0f, 1.0f);
    
    // Apply brightness setting and convert to PWM (0-255)
    uint8_t pwmValue = (uint8_t)(channelIntensities[i] * settings.brightness);
    ledDriver->setChannel(i, pwmValue);
  }
  
  // Update PWM outputs
  ledDriver->update();
  
  // Update noise offset for flicker
  noiseOffset++;
}

void LEDEffects::setBrightness(uint8_t brightness) {
  // Brightness is applied in render() method
  // This method is kept for API compatibility
}

void LEDEffects::renderCoreEffect(const AfterburnerSettings& settings, float throttle, float channelIntensities[4]) {
  // Get eased throttle value based on mode
  float easedThrottle = getEasedThrottle(throttle, settings.mode);
  
  // Calculate base intensity from start color (average RGB)
  float baseIntensity = calculateIntensityFromColor(settings.startColor);
  
  // All 4 channels active with base intensity proportional to throttle
  for (uint8_t i = 0; i < NUM_MOSFET_CHANNELS; i++) {
    channelIntensities[i] = easedThrottle * baseIntensity;
  }
  
  // Apply breathing effect (for Ease and Pulse modes)
  if (settings.mode == 1 || settings.mode == 2) {
    // Use speedMs to control breathing frequency
    float breathingSpeed = 1000.0f / (float)settings.speedMs;
    float breathing = 0.8f + 0.2f * sin(millis() * breathingSpeed * 0.001f);
    for (uint8_t i = 0; i < NUM_MOSFET_CHANNELS; i++) {
      channelIntensities[i] *= breathing;
    }
  }
}

void LEDEffects::renderAfterburnerOverlay(const AfterburnerSettings& settings, float throttle, float channelIntensities[4]) {
  // Calculate afterburner threshold
  float abThreshold = settings.abThreshold / 100.0f;
  
  if (throttle <= abThreshold) {
    return; // No afterburner effect
  }
  
  // Calculate afterburner intensity
  float abIntensity = (throttle - abThreshold) / (1.0f - abThreshold);
  abIntensity = constrain(abIntensity, 0.0f, 1.0f);
  
  // Calculate afterburner intensity from end color (average RGB)
  float abColorIntensity = calculateIntensityFromColor(settings.endColor);
  float abBoost = abIntensity * abColorIntensity;
  
  // Create rotating pattern for circular effect
  // Each channel gets phase offset for rotation
  float rotationSpeed = CIRCLE_ROTATION_SPEED * (1000.0f / (float)settings.speedMs);
  float time = millis() * rotationSpeed * 0.001f;  // Convert to seconds
  
  for (uint8_t i = 0; i < NUM_MOSFET_CHANNELS; i++) {
    // Create sine wave pattern with phase offset per channel
    // This creates a rotating effect around the circle
    float phase = (i * M_PI * 2.0f) / NUM_MOSFET_CHANNELS;  // 90° offset per channel (360°/4)
    float rotation = sin(time + phase);
    
    // Apply afterburner boost with rotation pattern
    // Rotation creates wave effect: 0.5 + 0.5*sin gives 0-1 range
    float rotationFactor = 0.5f + 0.5f * rotation;
    channelIntensities[i] += abBoost * rotationFactor;
  }
  
  // Pulse modulation for pulse mode
  if (settings.mode == 2) {
    float pulseFreq = 1000.0f / (float)settings.speedMs;
    float pulse = 0.6f + 0.4f * sin(millis() * pulseFreq * 0.001f);
    for (uint8_t i = 0; i < NUM_MOSFET_CHANNELS; i++) {
      channelIntensities[i] *= pulse;
    }
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

void LEDEffects::addFlicker(float channelIntensities[4], const AfterburnerSettings& settings) {
  // Apply subtle random variations to all channels
  // Use speedMs to control flicker speed (faster speed = faster flicker)
  float flickerSpeed = 1000.0f / (float)settings.speedMs;
  
  for (uint8_t i = 0; i < NUM_MOSFET_CHANNELS; i++) {
    if (channelIntensities[i] > 0) {
      // Generate pseudo-random flicker based on time and channel
      // Use millis() and channel index for deterministic randomness
      uint32_t seed = (millis() * flickerSpeed * 0.001f) + (i * 1000) + noiseOffset;
      float flicker = 1.0f + (sin(seed) * 0.03f);  // ±3% variation
      channelIntensities[i] = constrain(channelIntensities[i] * flicker, 0.0f, 1.0f);
    }
  }
}

void LEDEffects::addSparkles(float channelIntensities[4], float abIntensity, const AfterburnerSettings& settings) {
  // Add random brief intensity spikes on random channels
  // Use speedMs to control sparkle frequency (faster speed = more sparkles)
  float sparkleFrequency = 1000.0f / (float)settings.speedMs;
  float sparkleChance = abIntensity * 0.1f * sparkleFrequency;
  
  for (uint8_t i = 0; i < NUM_MOSFET_CHANNELS; i++) {
    // Random chance for sparkle on this channel
    if (random(1000) < (sparkleChance * 1000)) {
      channelIntensities[i] = min(1.0f, channelIntensities[i] + 0.3f);
    }
  }
}

float LEDEffects::calculateIntensityFromColor(uint8_t color[3]) {
  // Calculate intensity as average of RGB values
  // Returns normalized value (0.0 - 1.0)
  float intensity = (color[0] + color[1] + color[2]) / 3.0f / 255.0f;
  return constrain(intensity, 0.0f, 1.0f);
}
