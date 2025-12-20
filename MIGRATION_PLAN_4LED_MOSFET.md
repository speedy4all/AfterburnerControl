# Migration Plan: Pixel LEDs → 36 LED Circle with 4 MOSFET Channels

## Overview

This document outlines the migration from WS2812B pixel LED strips to 36x 5W 5V LEDs arranged in a circle, controlled via 4 MOSFET transistors with PWM. Each MOSFET controls 9 LEDs in parallel, with LEDs randomly distributed around the circle (not physically grouped). All LEDs remain active with varying intensities to create the afterburner effect.

---

## Hardware Changes

### Current Hardware

- **LED Control**: WS2812B pixel LED strip (configurable count, typically 45 LEDs)
- **Control Method**: Single GPIO pin with FastLED library
- **Power**: 5V via LED strip

### New Hardware

- **LED Control**: 36x 5W 5V LEDs arranged in a circle
- **Control Method**: 4 MOSFET transistors with PWM
  - **MOSFET 1**: Controls 9 LEDs (randomly distributed around circle)
  - **MOSFET 2**: Controls 9 LEDs (randomly distributed around circle)
  - **MOSFET 3**: Controls 9 LEDs (randomly distributed around circle)
  - **MOSFET 4**: Controls 9 LEDs (randomly distributed around circle)
- **GPIO Pins**: 4 PWM-capable GPIO pins (one per MOSFET gate)
- **Power**: 5V power supply capable of 180W total (36 LEDs × 5W each)
- **Physical Arrangement**: LEDs arranged in a circle, electrically grouped by MOSFET but physically distributed

### Recommended GPIO Pins (ESP32-C3 SuperMini)

- **MOSFET 1 (Channel 0)**: GPIO 2 (PWM capable)
- **MOSFET 2 (Channel 1)**: GPIO 3 (PWM capable)
- **MOSFET 3 (Channel 2)**: GPIO 4 (PWM capable)
- **MOSFET 4 (Channel 3)**: GPIO 5 (PWM capable)

**Note**: GPIO 1 is used for throttle input, GPIO 0 is typically used for boot/programming.

### LED Distribution Strategy

Since LEDs are randomly distributed per MOSFET around the circle, we cannot rely on physical position for spatial effects. Instead, we'll create intensity patterns that work with the random distribution:

- **All LEDs Active**: All 4 channels active at all times (when throttle > 0)
- **Intensity Modulation**: Vary intensity per channel to create visual effects
- **Circular Effects**: Create rotating patterns, waves, or gradients that work regardless of physical LED positions

---

## Firmware Changes

### 1. Remove FastLED Dependency

**File**: `firmware/platformio.ini`

- **Action**: Remove `fastled/FastLED@^3.10.2` from `lib_deps`
- **Reason**: No longer needed for pixel LED control

### 2. Update Constants

**File**: `firmware/src/constants.h`

- **Remove**: `LED_STRIP_PIN` definition
- **Add**:

  ```cpp
  #define MOSFET_1_PIN 2      // GPIO 2 - Channel 0
  #define MOSFET_2_PIN 3       // GPIO 3 - Channel 1
  #define MOSFET_3_PIN 4       // GPIO 4 - Channel 2
  #define MOSFET_4_PIN 5       // GPIO 5 - Channel 3
  #define NUM_MOSFET_CHANNELS 4
  #define NUM_LEDS_PER_CHANNEL 9
  #define TOTAL_LEDS 36
  #define PWM_FREQUENCY 5000   // 5kHz PWM frequency
  #define PWM_RESOLUTION 8     // 8-bit resolution (0-255)

  // Circular effect parameters
  #define CIRCLE_ROTATION_SPEED 0.001f  // Rotation speed multiplier
  ```

### 3. Update Settings Structure

**File**: `firmware/src/settings.h`

- **Remove**: `numLeds` (replaced by fixed 36 LEDs, 4 channels)
- **Keep**: `startColor` and `endColor` (repurposed for intensity mapping)
- **Modify**:
  - `startColor[3]` → Base intensity color (used to calculate base intensity)
  - `endColor[3]` → Afterburner intensity color (used to calculate afterburner intensity)
  - Use average RGB value as intensity: `intensity = (R + G + B) / 3`

**Recommended Approach**:

- Keep color structure for backward compatibility
- Use average RGB value as intensity: `intensity = (R + G + B) / 3`
- This allows color picker to still work but controls brightness/intensity
- Colors represent intensity levels, not actual colors (all LEDs are white/5W LEDs)

### 4. Create New LED Driver Module

**New File**: `firmware/src/led_driver.h` and `firmware/src/led_driver.cpp`

**Purpose**: Replace FastLED with direct PWM control

**Interface**:

```cpp
class LEDDriver {
private:
  uint8_t mosfetPins[4];           // GPIO pins for 4 MOSFETs
  uint8_t channelIntensities[4];   // 0-255 PWM values per channel
  uint8_t ledcChannels[4];         // LEDC channel assignments

public:
  LEDDriver();
  void begin();
  void setChannel(uint8_t channel, uint8_t intensity);  // channel 0-3, intensity 0-255
  void setAllChannels(uint8_t intensities[4]);
  void update();  // Apply PWM values (called automatically, but can be manual)
  void clear();   // Turn all LEDs off
  uint8_t getChannelIntensity(uint8_t channel);  // Get current intensity
};
```

**Implementation Notes**:

- Each MOSFET controls 9 LEDs in parallel
- All 9 LEDs on a channel receive the same PWM signal
- Use ESP32 LEDC (LED Control) hardware PWM for smooth control
- 4 independent PWM channels, one per MOSFET
- 8-bit resolution (0-255) matches current brightness range
- 5kHz frequency is good for LED dimming (avoids visible flicker)

### 5. Refactor LED Effects

**File**: `firmware/src/led_effects.h` and `firmware/src/led_effects.cpp`

**Major Changes**:

1. **Remove FastLED includes and CRGB types**
2. **Replace with LEDDriver**
3. **Map afterburner effect to 4 LEDs spatially**

**Circular Effect Strategy**:

Since LEDs are randomly distributed per MOSFET around the circle, we create intensity patterns that work regardless of physical position:

- **All Channels Active**: All 4 MOSFET channels active when throttle > 0
- **Intensity Modulation**: Vary intensity per channel to create visual effects
- **Circular Patterns**: Create rotating, pulsing, or wave effects

**Effect Mapping**:

- **Core Effect**: All channels active with base intensity proportional to throttle

  - Low throttle: All channels at low intensity (e.g., 20-30% of max)
  - Medium throttle: All channels at medium intensity (e.g., 50-60% of max)
  - High throttle: All channels at high intensity (e.g., 80-90% of max)
  - Base intensity calculated from `startColor` average RGB

- **Afterburner Overlay**:

  - When throttle > threshold, add intensity boost to all channels
  - Create rotating/wave pattern across channels for visual interest
  - Pulse mode: Modulate afterburner intensity with sine wave
  - Afterburner intensity calculated from `endColor` average RGB

- **Circular Rotation Effect**:

  - Create rotating intensity pattern across 4 channels
  - Simulates spinning afterburner effect
  - Rotation speed controlled by `speedMs` setting
  - Pattern: Channel intensities follow sine wave with phase offset

- **Breathing Effect**: Apply to all channels simultaneously (all pulse together)
- **Flicker Effect**: Apply random variations to all channels
- **Sparkles**: Random brief intensity spikes on random channels

**New render() method logic**:

```cpp
void LEDEffects::render(const AfterburnerSettings& settings, float throttle) {
  // Calculate base intensity from throttle and start color
  float easedThrottle = getEasedThrottle(throttle, settings.mode);

  // Calculate base intensity from start color (average RGB)
  float baseIntensity = (settings.startColor[0] + settings.startColor[1] + settings.startColor[2]) / 3.0f / 255.0f;

  // Calculate afterburner intensity from end color (average RGB)
  float abColorIntensity = (settings.endColor[0] + settings.endColor[1] + settings.endColor[2]) / 3.0f / 255.0f;

  // All 4 channels active with base intensity
  float channelIntensities[4];
  for (int i = 0; i < 4; i++) {
    channelIntensities[i] = easedThrottle * baseIntensity;
  }

  // Apply afterburner overlay
  float abThreshold = settings.abThreshold / 100.0f;
  if (throttle > abThreshold) {
    float abIntensity = (throttle - abThreshold) / (1.0f - abThreshold);
    float abBoost = abIntensity * abColorIntensity;

    // Create rotating pattern for circular effect
    // Each channel gets phase offset for rotation
    float rotationSpeed = CIRCLE_ROTATION_SPEED * (1000.0f / settings.speedMs);
    float time = millis() * rotationSpeed;

    for (int i = 0; i < 4; i++) {
      // Create sine wave pattern with phase offset per channel
      // This creates a rotating effect around the circle
      float phase = (i * M_PI * 2.0f) / 4.0f;  // 90° offset per channel
      float rotation = sin(time + phase);

      // Apply afterburner boost with rotation pattern
      // Rotation creates wave effect: 0.5 + 0.5*sin gives 0-1 range
      float rotationFactor = 0.5f + 0.5f * rotation;
      channelIntensities[i] += abBoost * rotationFactor;
    }

    // Pulse modulation for pulse mode
    if (settings.mode == 2) {
      float pulseFreq = 1000.0f / settings.speedMs;
      float pulse = 0.6f + 0.4f * sin(millis() * pulseFreq);
      for (int i = 0; i < 4; i++) {
        channelIntensities[i] *= pulse;
      }
    }
  }

  // Apply breathing effect (for Ease and Pulse modes)
  if (settings.mode == 1 || settings.mode == 2) {
    float breathingSpeed = 1000.0f / settings.speedMs;
    float breathing = 0.8f + 0.2f * sin(millis() * breathingSpeed);
    for (int i = 0; i < 4; i++) {
      channelIntensities[i] *= breathing;
    }
  }

  // Apply flicker (subtle random variations)
  for (int i = 0; i < 4; i++) {
    if (channelIntensities[i] > 0) {
      float flicker = 1.0f + (random(-15, 15) / 255.0f);
      channelIntensities[i] = constrain(channelIntensities[i] * flicker, 0.0f, 1.0f);
    }
  }

  // Apply sparkles during strong afterburner
  if (throttle > abThreshold && (throttle - abThreshold) > 0.3f) {
    float sparkleChance = (throttle - abThreshold) * 0.1f;
    for (int i = 0; i < 4; i++) {
      if (random(1000) < (sparkleChance * 1000)) {
        channelIntensities[i] = min(1.0f, channelIntensities[i] + 0.3f);
      }
    }
  }

  // Apply brightness setting and convert to PWM
  for (int i = 0; i < 4; i++) {
    uint8_t pwmValue = (uint8_t)(channelIntensities[i] * settings.brightness);
    ledDriver.setChannel(i, pwmValue);
  }

  ledDriver.update();
}
```

**Key Features**:

- **All Channels Active**: All 4 MOSFETs active when throttle > 0
- **Rotating Pattern**: Sine wave with phase offset creates rotating effect
- **Afterburner Boost**: Additional intensity when throttle exceeds threshold
- **Circular Effect**: Rotation pattern works regardless of LED physical positions

### 6. Update Main.cpp

**File**: `firmware/src/main.cpp`

**Changes**:

- Remove `LED_STRIP_PIN` initialization
- Initialize 4 MOSFET pins as PWM outputs via LEDDriver
- Remove `numLeds` from `ledEffects.begin()` call
- Update LED effects initialization

```cpp
// In setup():
// LEDDriver handles pin initialization
ledDriver.begin();  // Sets up 4 PWM channels

ledEffects.begin();  // No longer needs numLeds parameter
```

### 7. Update BLE Service

**File**: `firmware/src/ble_service.h` and `firmware/src/ble_service.cpp`

**Changes**:

- **Keep**: `NUM_LEDS_UUID` characteristic for backward compatibility
- **Modify**: `handleNumLedsWrite()` - Make it a no-op (accepts writes but ignores them)
- **Modify**: `updateCharacteristicValues()` - Set numLeds to fixed value (36) or 0
- **Optional**: Add `HARDWARE_VERSION_UUID` characteristic
  - Value: `2` (indicates new hardware type)
  - Helps mobile app auto-detect hardware type
  - If not present, app falls back to NUM_LEDS detection

**Implementation**:

```cpp
// In handleNumLedsWrite():
void AfterburnerBLEService::handleNumLedsWrite(BLECharacteristic* pCharacteristic) {
  // Accept write for backward compatibility, but ignore value
  // New hardware has fixed 36 LEDs, 4 channels
  Serial.println("BLE: Num LEDs write received (ignored - fixed at 36 LEDs, 4 channels)");
  // No-op: Don't update settings
}

// In updateCharacteristicValues():
// Set numLeds to 36 or 0 to indicate fixed configuration
uint16_t numLedsValue = 36;  // Or 0 to indicate "not applicable"
uint8_t numLedsBytes[2];
uint16ToBytes(numLedsValue, numLedsBytes);
pNumLedsCharacteristic->setValue(numLedsBytes, 2);
```

**Note**: Keeping the characteristic allows old apps to connect without errors, even though the setting is ignored.

---

## Mobile Application Changes

### 1. Update Type Definitions

**File**: `AfterburnerControl/src/ble/device.ts`

**Changes**:

- **Keep**: `numLeds` in `AfterburnerSettings` interface (for backward compatibility)
- **Add**: Hardware type detection and management
- **Keep**: `startColor` and `endColor` (repurposed for intensity control)
- **Update**: Documentation to reflect that colors control intensity

```typescript
// Hardware types
export enum HardwareType {
  LEGACY = 1, // Pixel LEDs with configurable numLeds
  NEW = 2, // 36 LED circle with 4 MOSFET channels
}

export interface AfterburnerSettings {
  mode: number;
  startColor: { r: number; g: number; b: number }; // Base intensity
  endColor: { r: number; g: number; b: number }; // Afterburner intensity
  speedMs: number;
  brightness: number;
  numLeds: number; // Kept for backward compatibility (ignored for new hardware)
  abThreshold: number;
}

// Hardware info interface
export interface HardwareInfo {
  type: HardwareType;
  numLeds?: number; // For legacy hardware
  numChannels?: number; // For new hardware (always 4)
  totalLeds?: number; // For new hardware (always 36)
}
```

### 2. Add Hardware Type Detection

**File**: `AfterburnerControl/src/ble/device.ts`

**New Functions**:

```typescript
// Detect hardware type on connection
export async function detectHardwareType(): Promise<HardwareType> {
  try {
    // Try to read hardware version characteristic (if exists)
    try {
      const versionData = await bleManager.readCharacteristic(
        HARDWARE_VERSION_UUID
      );
      if (versionData && versionData.length > 0) {
        const version = versionData[0];
        return version === 2 ? HardwareType.NEW : HardwareType.LEGACY;
      }
    } catch (error) {
      // Hardware version characteristic doesn't exist, try fallback
    }

    // Fallback: Try to read NUM_LEDS characteristic
    try {
      const numLedsBytes = await bleManager.readCharacteristic(
        BLE_UUIDS.NUM_LEDS
      );
      if (numLedsBytes && numLedsBytes.length >= 2) {
        const numLeds = numLedsBytes[0] | (numLedsBytes[1] << 8);
        // New hardware returns 36 or 0, legacy returns configurable value (1-300)
        if (numLeds === 36 || numLeds === 0) {
          return HardwareType.NEW;
        } else if (numLeds >= 1 && numLeds <= 300) {
          return HardwareType.LEGACY;
        }
      }
    } catch (error) {
      // If NUM_LEDS read fails, assume new hardware (graceful degradation)
      console.warn("Could not detect hardware type, assuming NEW hardware");
      return HardwareType.NEW;
    }

    // Default to new hardware if detection fails
    return HardwareType.NEW;
  } catch (error) {
    console.error("Hardware type detection failed:", error);
    return HardwareType.NEW; // Default to new hardware
  }
}

// Get hardware info
export async function getHardwareInfo(): Promise<HardwareInfo> {
  const type = await detectHardwareType();

  if (type === HardwareType.LEGACY) {
    try {
      const numLedsBytes = await bleManager.readCharacteristic(
        BLE_UUIDS.NUM_LEDS
      );
      const numLeds = numLedsBytes[0] | (numLedsBytes[1] << 8);
      return { type, numLeds };
    } catch (error) {
      return { type, numLeds: 45 }; // Default
    }
  } else {
    return { type, numChannels: 4, totalLeds: 36 };
  }
}
```

### 3. Update LED Count Control (Conditional Display)

**File**: `AfterburnerControl/src/screens/AfterburnerScreen.tsx`

**Changes**:

- **Add**: Hardware type state
- **Add**: Hardware detection on connection
- **Conditional**: Show "Number of LEDs" only for legacy hardware
- **Conditional**: Show hardware info (36 LEDs, 4 channels) for new hardware
- **Keep**: `sendNumLeds()` function (only active for legacy hardware)
- **Keep**: `numLeds` in input values state (for legacy hardware)
- **Update**: `pushAllSettings()` to conditionally send numLeds

### 4. Update Color Input Labels

**File**: `AfterburnerControl/src/screens/AfterburnerScreen.tsx`

**Changes**:

- **Update**: "Start Color" label → "Base Intensity Color"
- **Update**: "End Color" label → "Afterburner Intensity Color"
- **Add**: Helper text explaining that RGB values control intensity (average of R+G+B)

**File**: `AfterburnerControl/src/components/ColorInput.tsx`

- **Optional**: Add intensity preview (show calculated intensity value)

### 5. Update Default Values and UUIDs

**File**: `AfterburnerControl/src/ble/uuids.ts`

**Changes**:

- **Keep**: `NUM_LEDS` UUID (for backward compatibility)
- **Add**: `HARDWARE_VERSION` UUID (optional, for auto-detection)
- **Keep**: `numLeds: 45` in defaults (for legacy hardware compatibility)

```typescript
export const BLE_UUIDS = {
  // ... existing UUIDs ...
  NUM_LEDS: "b5f9a006-2b6c-4f6a-93b1-2f1f5f9ab006",
  HARDWARE_VERSION: "b5f9a013-2b6c-4f6a-93b1-2f1f5f9ab013", // New, optional
  // ... rest of UUIDs ...
};
```

**File**: `AfterburnerControl/src/ble/uuids.ts` (or wherever DEFAULT_VALUES is defined)

**Changes**:

- **Keep**: `numLeds: 45` in defaults (for legacy hardware)
- **Keep**: Color defaults (they'll work as intensity)

### 6. Update BLE Communication

**File**: `AfterburnerControl/src/ble/device.ts`

**Changes**:

- **Keep**: `writeNumLeds()` function (for legacy hardware)
- **Keep**: `numLeds` reading from `readSettings()` (for backward compatibility)
- **Update**: `pushAllSettings()` to conditionally include numLeds based on hardware type
- **Add**: Hardware type parameter to functions that need it

```typescript
// Update pushAllSettings to be hardware-aware
export async function pushAllSettings(
  settings: AfterburnerSettings,
  hardwareType?: HardwareType
): Promise<void> {
  // If hardware type not provided, detect it
  if (!hardwareType) {
    hardwareType = await detectHardwareType();
  }

  // Write all settings
  await writeMode(settings.mode);
  await writeStartColor(settings.startColor);
  await writeEndColor(settings.endColor);
  await writeSpeedMs(settings.speedMs);
  await writeBrightness(settings.brightness);

  // Only write numLeds for legacy hardware
  if (hardwareType === HardwareType.LEGACY) {
    await writeNumLeds(settings.numLeds);
  }

  await writeAbThreshold(settings.abThreshold);

  // Save settings to ESP32 flash memory
  await savePreset();
}
```

---

## Testing Plan

### Hardware Testing

1. **PWM Verification**: Test each LED channel independently

   - Verify PWM frequency and resolution
   - Test full range (0-255)
   - Check for flicker at various intensities

2. **Circular Effect**: Test afterburner effect

   - Low throttle: All channels at low intensity
   - Medium throttle: All channels at medium intensity
   - High throttle: All channels at high intensity
   - Afterburner: All channels active with rotating pattern

3. **Effect Testing**:
   - Breathing effect on all modes
   - Pulse mode afterburner modulation
   - Flicker effect
   - Sparkles during afterburner

### Software Testing

1. **BLE Compatibility**: Ensure app can still connect and control
2. **Settings Persistence**: Verify settings save/load correctly
3. **Hardware Detection**: Test auto-detection on both legacy and new hardware
4. **Backward Compatibility**:
   - Test legacy hardware with updated app
   - Test new hardware with updated app
   - Test old app version connecting to new hardware (graceful degradation)

---

## Migration Steps (Implementation Order)

### Phase 1: Firmware Foundation

1. ✅ Create migration plan (this document)
2. Remove FastLED dependency from platformio.ini
3. Create LEDDriver module (led_driver.h/cpp)
4. Update constants.h with new pin definitions
5. Test LEDDriver independently

### Phase 2: Core Refactoring

6. Refactor led_effects.h/cpp to use LEDDriver
7. Implement spatial distribution logic
8. Update main.cpp initialization
9. Remove numLeds from settings (or deprecate)

### Phase 3: BLE Updates

10. Keep NUM_LEDS characteristic (for backward compatibility)
11. Make NUM_LEDS handler a no-op for new hardware
12. Add optional HARDWARE_VERSION characteristic
13. Test BLE communication

### Phase 4: Mobile App Updates

14. Add hardware type detection functions
15. Add hardware type state management
16. Update TypeScript interfaces (keep numLeds)
17. Add conditional UI rendering based on hardware type
18. Update color input labels and help text
19. Update BLE communication to be hardware-aware

### Phase 5: Integration & Testing

17. Full system integration test
18. Hardware verification
19. Effect validation
20. Performance testing

---

## Backward Compatibility Considerations

### ✅ Selected: Graceful Degradation

Since the old hardware will still be in use, we need to support both hardware types:

**Hardware Types**:

- **Type 1 (Legacy)**: Pixel LEDs (WS2812B) with configurable `numLeds`
- **Type 2 (New)**: 36 LED circle with 4 MOSFET channels (fixed configuration)

**Implementation Strategy**:

1. **Firmware**: Keep `NUM_LEDS` characteristic but make it optional

   - **Legacy firmware**: `NUM_LEDS` characteristic works normally
   - **New firmware**: `NUM_LEDS` characteristic exists but is ignored (no-op)
   - This allows old apps to connect without errors

2. **Mobile App**: Add hardware type detection and UI adaptation

   - **Auto-detection**: Try to read `NUM_LEDS` characteristic on connection
     - If readable and responds: Legacy hardware (Type 1)
     - If missing or returns error: New hardware (Type 2)
   - **Manual override**: Add settings option to manually select hardware type
   - **UI adaptation**: Show/hide controls based on hardware type
     - Legacy: Show "Number of LEDs" control
     - New: Hide "Number of LEDs", show hardware info (36 LEDs, 4 channels)

3. **BLE Characteristic**: Add optional hardware version characteristic (optional enhancement)
   - UUID: `HARDWARE_VERSION_UUID` (new)
   - Value: `1` = Legacy, `2` = New hardware
   - If not present, fall back to auto-detection via `NUM_LEDS`

**Benefits**:

- Old hardware continues to work with updated app
- New hardware works with updated app
- Old app versions can still connect (graceful degradation)
- Clean migration path

---

## Notes & Considerations

### Power Requirements

- **36x 5W LEDs = 180W total**
- Ensure power supply can handle 180W + overhead (recommend 200W+ supply)
- Consider heat dissipation for MOSFETs (each handling ~45W)
- Add appropriate current-limiting resistors if needed
- **Important**: Verify MOSFETs can handle current (9 LEDs × ~1A each = ~9A per MOSFET)
- Use appropriate MOSFETs rated for at least 10-15A continuous current

### MOSFET Selection

- Use logic-level MOSFETs (Vgs < 3.3V)
- **Current Rating**: Must handle ~9-10A per MOSFET (9 LEDs × ~1A each)
- **Recommended**:
  - IRLB8743 (30A, logic-level)
  - IRLB8748 (30A, logic-level)
  - IRLZ44N (47A, but verify logic-level operation)
  - Or similar high-current logic-level MOSFETs
- Add gate resistors (100-220Ω) to prevent ringing
- Consider flyback diodes if driving inductive loads
- **Heat Sinking**: MOSFETs will dissipate heat - add heat sinks if needed
- **Wiring**: Use appropriate wire gauge for 9-10A current per channel

### PWM Frequency

- 5kHz is a good starting point
- Too low (< 1kHz): Visible flicker
- Too high (> 20kHz): May cause EMI issues
- Test and adjust based on hardware

### Circular Effect Tuning

- The rotation speed can be adjusted via `speedMs` setting
- Rotation pattern phase offsets can be modified for different effects
- Consider making rotation direction configurable (clockwise/counter-clockwise)
- Test with actual hardware to find best visual effect
- **Effect Options**:
  - **Rotating Wave**: Current sine wave with phase offset (recommended)
  - **Pulsing All**: All channels pulse together (simpler, less dynamic)
  - **Sequential**: Channels light up in sequence (requires position knowledge)
  - **Random Flicker**: Random intensity variations (more chaotic)

---

## File Change Summary

### Files to Modify

- `firmware/platformio.ini` - Remove FastLED
- `firmware/src/constants.h` - Update pin definitions
- `firmware/src/settings.h` - Remove/deprecate numLeds
- `firmware/src/main.cpp` - Update initialization
- `firmware/src/led_effects.h` - Complete refactor
- `firmware/src/led_effects.cpp` - Complete refactor
- `firmware/src/ble_service.h` - Keep NUM_LEDS, add optional HARDWARE_VERSION
- `firmware/src/ble_service.cpp` - Make NUM_LEDS no-op, add hardware version handler
- `AfterburnerControl/src/ble/device.ts` - Add hardware detection, keep numLeds
- `AfterburnerControl/src/ble/uuids.ts` - Add HARDWARE_VERSION UUID
- `AfterburnerControl/src/screens/AfterburnerScreen.tsx` - Add hardware detection, conditional UI

### Files to Create

- `firmware/src/led_driver.h` - New LED driver header
- `firmware/src/led_driver.cpp` - New LED driver implementation
- `AfterburnerControl/src/types/hardware.ts` - Hardware type definitions (optional, can be in device.ts)

### Files to Delete

- None (keep for reference, but code will be unused)

---

## Success Criteria

✅ All 4 MOSFET channels can be controlled independently via PWM  
✅ All 36 LEDs are active and respond to throttle (9 LEDs per channel)  
✅ Afterburner effect creates rotating/wave pattern around the circle  
✅ All existing modes (Linear, Ease, Pulse) work correctly  
✅ Breathing, flicker, and sparkle effects work  
✅ Circular rotation effect visible during afterburner  
✅ BLE communication works with updated mobile app  
✅ Settings persist correctly  
✅ Legacy hardware works with updated app (backward compatibility)  
✅ New hardware works with updated app  
✅ Hardware type auto-detection works correctly  
✅ UI adapts based on hardware type  
✅ No visible PWM flicker  
✅ Smooth throttle response  
✅ Power supply handles 180W+ load without issues  
✅ MOSFETs stay within temperature limits

---

## Timeline Estimate

- **Phase 1** (Firmware Foundation): 2-3 hours
- **Phase 2** (Core Refactoring): 4-6 hours
- **Phase 3** (BLE Updates): 1-2 hours
- **Phase 4** (Mobile App Updates): 3-4 hours (includes hardware detection)
- **Phase 5** (Integration & Testing): 3-4 hours

**Total**: ~13-19 hours of development time (slightly increased due to backward compatibility)

---

## Questions to Resolve

1. **Color to Intensity Mapping**:

   - ✅ **RESOLVED**: Use average RGB: `(R+G+B)/3`
   - This allows color picker to work intuitively

2. **Circular Effect Pattern**:

   - ✅ **RESOLVED**: Rotating sine wave with phase offset
   - Creates wave effect that works regardless of LED positions
   - Speed controlled by `speedMs` setting

3. **Backward Compatibility**:

   - ✅ **RESOLVED**: Graceful degradation
   - Keep `NUM_LEDS` characteristic in firmware (no-op for new hardware)
   - Auto-detect hardware type in mobile app
   - Conditionally show/hide UI elements based on hardware type
   - Old hardware continues to work with updated app

4. **Rotation Direction**:

   - Clockwise or counter-clockwise?
   - **Recommendation**: Make configurable via BLE (optional enhancement)

5. **Effect Intensity**:
   - How strong should the rotation effect be?
   - **Recommendation**: Start with moderate rotation (0.3-0.5 multiplier), tune based on visual feedback

---

**Document Version**: 1.1  
**Last Updated**: 2024  
**Status**: Ready for Implementation  
**Changes**: Updated for graceful degradation with hardware type detection
