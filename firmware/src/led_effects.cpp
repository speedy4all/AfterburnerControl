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

void LEDEffects::begin(uint16_t totalLedCount) {
  if (leds) {
    delete[] leds;
  }
  
  // Store LEDs per ring (total should be numLeds * 2 for dual turbines)
  numLeds = totalLedCount / 2;
  uint16_t actualTotalLeds = numLeds * 2;  // Ensure we use exactly 2 rings
  
  leds = new CRGB[actualTotalLeds];
  
  FastLED.addLeds<WS2812B, LED_STRIP_PIN, GRB>(leds, actualTotalLeds);
  FastLED.setBrightness(200);
  FastLED.clear();
  FastLED.show();
}

void LEDEffects::update(uint16_t newTotalLedCount) {
  uint16_t newNumLeds = newTotalLedCount / 2;
  if (newNumLeds != numLeds) {
    begin(newTotalLedCount);
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

// Helper methods for dual-ring support
bool LEDEffects::isRing2(uint16_t ledIndex) const {
  return ledIndex >= numLeds;
}

uint16_t LEDEffects::getRingLocalIndex(uint16_t ledIndex) const {
  return isRing2(ledIndex) ? (ledIndex - numLeds) : ledIndex;
}

float LEDEffects::getRingPosition(uint16_t ledIndex) const {
  uint16_t localIndex = getRingLocalIndex(ledIndex);
  float position = (float)localIndex / numLeds;
  // Reverse spatial position for ring 2 to create contrasting pattern
  return isRing2(ledIndex) ? (1.0f - position) : position;
}

void LEDEffects::renderCoreEffect(const AfterburnerSettings& settings, float throttle) {
  // Get eased throttle value based on mode
  float easedThrottle = getEasedThrottle(throttle, settings.mode);
  
  // Create start and end colors
  CRGB startColor = CRGB(settings.startColor[0], settings.startColor[1], settings.startColor[2]);
  CRGB endColor = CRGB(settings.endColor[0], settings.endColor[1], settings.endColor[2]);
  
  // Use constant brightness from settings (full brightness for color rendering)
  // FastLED.setBrightness(settings.brightness) handles overall brightness control
  uint8_t baseBrightness = 255;  // Full brightness - constant, not throttle-dependent
  
  // Render each LED (both rings: total = numLeds * 2)
  uint16_t totalLeds = numLeds * 2;
  
  // Special handling for Mode 0 (Linear mode): Random flickering LEDs
  if (settings.mode == 0) {
    // Calculate target percentage of LEDs that should be lit (with minimum at idle)
    float litPercentage = throttle;
    litPercentage = constrain(litPercentage, 0.20f, 1.0f);  // 20% minimum at idle, up to 100%
    
    // Calculate threshold for noise-based selection
    // Lower threshold = more LEDs lit, higher threshold = fewer LEDs lit
    // We want: at 0.20 throttle -> ~20% lit, at 1.0 throttle -> ~100% lit
    uint8_t noiseThreshold = (uint8_t)(255 - (255 * litPercentage));
    
    // Calculate flicker speed based on speedMs setting (faster flickering)
    // Use multiplier to speed up the animation - faster speedMs = faster flicker
    float flickerSpeedMultiplier = 5.0f;  // Speed multiplier for faster flickering
    float flickerSpeed = (1000.0f / (float)settings.speedMs) * flickerSpeedMultiplier;
    uint32_t timeOffset = (uint32_t)(millis() * flickerSpeed);
    
    for (uint16_t i = 0; i < totalLeds; i++) {
      bool ring2 = isRing2(i);
      uint16_t localIndex = getRingLocalIndex(i);
      
      // Generate independent noise for this LED (time-based, so it flickers)
      // Use different seed offsets for each ring to ensure independence
      uint32_t seedOffset = ring2 ? 10000 : 0;
      uint8_t noise = inoise8((localIndex * 37 + seedOffset), timeOffset + (localIndex * 13));
      
      // If noise exceeds threshold, LED is lit
      if (noise > noiseThreshold) {
        // For color: use raw throttle (not eased) to ensure startColor at idle
        // At throttle = 0, we want startColor; at throttle = 1, we want endColor
        CRGB color = lerpColor(startColor, endColor, throttle);
        
        // Apply base brightness (full brightness for lit LEDs)
        color.nscale8(baseBrightness);
        
        // Add flicker effect for extra realism
        addFlicker(i, 35, settings);
        
        // Set the LED
        leds[i] = color;
      } else {
        // LED is off
        leds[i] = CRGB::Black;
      }
    }
  } else {
    // Mode 1 (Ease) and Mode 2 (Pulse): Original behavior
    for (uint16_t i = 0; i < totalLeds; i++) {
      bool ring2 = isRing2(i);
      
      // Calculate breathing effect with phase offset for ring 2 (enhanced visibility)
      uint8_t currentBrightness = baseBrightness;
      if (settings.mode == 1 || settings.mode == 2) {
        // Use speedMs to control breathing frequency (Ease and Pulse modes)
        float breathingSpeed = 1000.0f / (float)settings.speedMs;
        // Add 180° phase offset for ring 2 to create contrasting effect
        float phaseOffset = ring2 ? M_PI : 0.0f;
        // Enhanced breathing effect: 0.7 to 1.0 range (30% variation for better visibility)
        float breathing = 0.7f + 0.3f * sin(millis() * breathingSpeed + phaseOffset);
        currentBrightness = (uint8_t)(baseBrightness * breathing);
      }
      
      // Interpolate color based on eased throttle (SAME for both rings)
      // This creates the transition from 0% to 100% throttle
      CRGB color = lerpColor(startColor, endColor, easedThrottle);
      
      // Apply breathing brightness effect (for Ease and Pulse modes)
      color.nscale8(currentBrightness);
      
      // Add flicker effect with increased intensity for better visibility (will have phase offset in addFlicker)
      addFlicker(i, 35, settings);  // Increased from 20 to 35 for better visibility
      
      // Set the LED
      leds[i] = color;
    }
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
  
  // Render afterburner effect for both rings
  uint16_t totalLeds = numLeds * 2;
  for (uint16_t i = 0; i < totalLeds; i++) {
    bool ring2 = isRing2(i);
    float position = getRingPosition(i);  // Already reversed for ring 2
    
    // Calculate spatial profile (stronger in center for ring 1, reversed for ring 2)
    float spatialProfile = 0.65f + 0.35f * sin(2.0f * M_PI * position);
    
    // Apply pulse modulation for Pulse mode with phase offset for ring 2
    float currentAbIntensity = abIntensity;
    if (settings.mode == 2) {
      // Use speedMs to control pulse frequency (faster speed = faster pulse)
      float pulseFrequency = 1000.0f / (float)settings.speedMs;
      // Add 180° phase offset for ring 2 to create contrasting pulse
      float phaseOffset = ring2 ? M_PI : 0.0f;
      float pulse = 0.6f + 0.4f * sin(millis() * pulseFrequency + phaseOffset);
      currentAbIntensity *= pulse;
    }
    
    // Blend afterburner colors based on throttle (SAME for both rings)
    CRGB abColor = lerpColor(abCoreColor1, abCoreColor2, throttle);
    
    // Scale by intensity and spatial profile
    uint8_t abBrightness = (uint8_t)(255 * currentAbIntensity * spatialProfile);
    abColor.nscale8(abBrightness);
    
    // Add to existing LED color
    leds[i] += abColor;
  }
  
  // Add sparkles when afterburner is strong (independent per ring)
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
  bool ring2 = isRing2(ledIndex);
  uint16_t localIndex = getRingLocalIndex(ledIndex);
  
  // Generate noise-based flicker using FastLED noise functions
  // Use speedMs to control flicker speed (faster speed = faster flicker)
  float flickerSpeed = 1000.0f / (float)settings.speedMs;
  
  // Add phase offset for ring 2 to create independent flicker pattern
  uint32_t timeOffset = ring2 ? 1000 : 0;  // Different time offset for ring 2
  uint8_t noise = inoise8(localIndex * 12, (millis() * flickerSpeed + localIndex * 7) * 8 + noiseOffset + timeOffset);
  
  // Map noise to flicker range (enhanced for better visibility during day)
  int8_t flicker = map(noise, 0, 255, -intensity, intensity);
  
  // Apply flicker to LED (additive for better visibility)
  leds[ledIndex].addToRGB(flicker);
}

void LEDEffects::addSparkles(float abIntensity, const AfterburnerSettings& settings) {
  // Add random white sparkles with independent patterns for each ring
  // Use speedMs to control sparkle frequency (faster speed = more sparkles)
  float sparkleFrequency = 1000.0f / (float)settings.speedMs;
  uint16_t sparkleChance = (uint16_t)(abIntensity * 50 * sparkleFrequency);
  
  uint16_t totalLeds = numLeds * 2;
  for (uint16_t i = 0; i < totalLeds; i++) {
    // Use LED index and ring offset to create independent sparkle patterns
    // Each ring gets different sparkle timing based on its index
    uint32_t sparkleSeed = (millis() * sparkleFrequency) + (i * 17) + (isRing2(i) ? 5000 : 0);
    if ((sparkleSeed % 1000) < sparkleChance) {
      uint8_t sparkleIntensity = 50 + (sparkleSeed % 100);  // 50-150 range
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
