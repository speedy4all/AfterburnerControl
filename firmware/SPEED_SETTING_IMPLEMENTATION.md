# Speed Setting Implementation

## Overview

The `speedMs` setting (100-5000ms) in the Afterburner firmware now controls the animation timing for various LED effects, making it a functional and useful control parameter.

## What the Speed Setting Controls

### 1. **Pulse Mode Afterburner Effects** (Mode 2)

- **Function**: Controls how fast the afterburner effect pulses
- **Formula**: `pulseFrequency = 1000.0f / speedMs`
- **Examples**:
  - 100ms = Very fast pulse (10Hz) - Rapid pulsing
  - 1200ms = Medium pulse (0.83Hz) - Normal pulsing
  - 5000ms = Slow pulse (0.2Hz) - Slow, gentle pulsing

### 2. **Breathing Effect** (Modes 1 & 2)

- **Function**: Controls the breathing animation speed for the main LED strip
- **Formula**: `breathingSpeed = 1000.0f / speedMs`
- **Effect**: Creates a subtle breathing effect where brightness gently rises and falls
- **Examples**:
  - 100ms = Rapid breathing - Very fast brightness changes
  - 1200ms = Normal breathing - Comfortable breathing rate
  - 5000ms = Slow breathing - Very slow, gentle breathing

### 3. **Flicker Effect**

- **Function**: Controls how fast the flicker animation cycles
- **Formula**: `flickerSpeed = 1000.0f / speedMs`
- **Effect**: Adds realistic flicker to simulate flame/engine effects
- **Examples**:
  - 100ms = Fast flicker - Rapid, intense flickering
  - 1200ms = Normal flicker - Realistic flicker rate
  - 5000ms = Slow flicker - Gentle, slow flickering

### 4. **Sparkle Effect**

- **Function**: Controls sparkle frequency during afterburner activation
- **Formula**: `sparkleFrequency = 1000.0f / speedMs`
- **Effect**: Adds white sparkles when afterburner is strong
- **Examples**:
  - 100ms = Many sparkles - Dense sparkle effect
  - 1200ms = Normal sparkles - Balanced sparkle density
  - 5000ms = Few sparkles - Sparse, occasional sparkles

## Technical Implementation

### Code Changes Made

1. **Pulse Mode** (`led_effects.cpp:110`):

   ```cpp
   // Before (hardcoded):
   float pulse = 0.6f + 0.4f * sin(millis() * 0.12f);

   // After (speed-controlled):
   float pulseFrequency = 1000.0f / (float)settings.speedMs;
   float pulse = 0.6f + 0.4f * sin(millis() * pulseFrequency);
   ```

2. **Breathing Effect** (`led_effects.cpp:80-85`):

   ```cpp
   // New breathing effect for Ease and Pulse modes
   if (settings.mode == 1 || settings.mode == 2) {
     float breathingSpeed = 1000.0f / (float)settings.speedMs;
     float breathing = 0.8f + 0.2f * sin(millis() * breathingSpeed);
     baseBrightness = (uint8_t)(baseBrightness * breathing);
   }
   ```

3. **Flicker Effect** (`led_effects.cpp:150-155`):

   ```cpp
   // Before (hardcoded):
   uint8_t noise = inoise8(ledIndex * 12, (millis() + ledIndex * 7) * 8 + noiseOffset);

   // After (speed-controlled):
   float flickerSpeed = 1000.0f / (float)settings.speedMs;
   uint8_t noise = inoise8(ledIndex * 12, (millis() * flickerSpeed + ledIndex * 7) * 8 + noiseOffset);
   ```

4. **Sparkle Effect** (`led_effects.cpp:170-175`):

   ```cpp
   // Before (hardcoded):
   if (random(1000) < (abIntensity * 50)) {

   // After (speed-controlled):
   float sparkleFrequency = 1000.0f / (float)settings.speedMs;
   uint16_t sparkleChance = (uint16_t)(abIntensity * 50 * sparkleFrequency);
   if (random(1000) < sparkleChance) {
   ```

### Function Signature Updates

- `addFlicker()` now takes `const AfterburnerSettings& settings` parameter
- `addSparkles()` now takes `const AfterburnerSettings& settings` parameter
- All function calls updated to pass the settings object

## User Experience

### Speed Ranges and Effects

| Speed Setting | Animation Speed | User Experience                                          |
| ------------- | --------------- | -------------------------------------------------------- |
| **100ms**     | Very Fast       | Rapid, intense animations - Good for high-energy effects |
| **500ms**     | Fast            | Quick animations - Good for dynamic effects              |
| **1200ms**    | Normal          | Default speed - Balanced, comfortable animations         |
| **2500ms**    | Slow            | Gentle animations - Good for relaxed effects             |
| **5000ms**    | Very Slow       | Very slow animations - Good for subtle, ambient effects  |

### Mode-Specific Behavior

- **Mode 0 (Linear)**: No breathing effect, speed only affects flicker and sparkles
- **Mode 1 (Ease)**: Breathing effect + speed-controlled flicker and sparkles
- **Mode 2 (Pulse)**: Breathing effect + speed-controlled pulse, flicker, and sparkles

## Benefits

1. **User Control**: Users can now customize animation speed to their preference
2. **Performance**: Faster speeds for high-energy situations, slower for ambient use
3. **Consistency**: All animation effects now use the same speed setting
4. **Realism**: More realistic engine/flame effects with adjustable timing
5. **Flexibility**: Different speeds work better in different lighting conditions

## Future Enhancements

Potential improvements could include:

- Speed presets (Slow, Normal, Fast, Turbo)
- Speed ramping for smooth transitions
- Mode-specific speed ranges
- Speed synchronization with throttle input
- Speed-based color temperature changes

## Verification

To verify the speed setting:

1. Set different speed values via the mobile app (100ms to 5000ms)
2. Observe changes in:
   - Pulse mode afterburner pulsing rate
   - Breathing effect speed
   - Flicker animation speed
   - Sparkle frequency during afterburner
3. Verify that faster speeds create more rapid animations
4. Verify that slower speeds create more gentle animations
