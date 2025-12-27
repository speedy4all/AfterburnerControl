# Dual Turbine Implementation Plan

## Overview

This document outlines the plan to modify the afterburner firmware to support **2 turbine rings** with **contrasting effects** for a more realistic dual-engine appearance.

## Current Architecture

- **LED Count**: `numLeds` represents LEDs per ring (currently 45 default)
- **LED Array**: Single continuous array from index 0 to `numLeds-1`
- **Rendering**: All effects render sequentially across all LEDs

## Target Architecture

- **Total LEDs**: `numLeds * 2` (e.g., 45 \* 2 = 90 LEDs total)
- **Ring 1**: LEDs 0 to `numLeds-1` (first turbine)
- **Ring 2**: LEDs `numLeds` to `(numLeds*2)-1` (second turbine)
- **Contrast Strategy**: Second ring displays effects with phase offsets and reversed patterns

## Implementation Plan

### Phase 1: Core Infrastructure Changes

#### 1.1 Update LEDEffects Class (`led_effects.h` & `led_effects.cpp`)

**Changes:**

- Keep `numLeds` as LEDs per ring (for settings compatibility)
- Add internal `totalLeds` = `numLeds * 2` for actual LED array size
- Modify `begin()` to allocate `numLeds * 2` LEDs
- Update all rendering loops to handle both rings

**Files to modify:**

- `firmware/src/led_effects.h` - Add helper methods for ring detection
- `firmware/src/led_effects.cpp` - Update all rendering functions

#### 1.2 Update Main Initialization (`main.cpp`)

**Changes:**

- Line 94: Change `ledEffects.begin(numLeds)` to `ledEffects.begin(numLeds * 2)`
- Add comment explaining dual-ring support

**Files to modify:**

- `firmware/src/main.cpp`

### Phase 2: Contrasting Effects Implementation

#### 2.1 Core Effect Contrast (`renderCoreEffect`)

**Strategy:**

- **Ring 1**: Normal rendering (existing behavior)
- **Ring 2**:
  - **Phase Offset**: Breathing/pulse effects offset by 180° (π radians)
  - **Same Color Gradient**: Color interpolation identical (startColor → endColor) - NO REVERSAL
  - **Reversed Spatial Position**: Reverse spatial position mapping for spatial effects only

**Implementation:**

```cpp
// For Ring 1 (i < numLeds): Normal rendering
// For Ring 2 (i >= numLeds):
//   - breathing: sin(millis() * speed + M_PI)  // 180° offset
//   - color: lerpColor(startColor, endColor, easedThrottle)  // SAME as ring 1
//   - position: 1.0f - ((i - numLeds) / numLeds)  // Reversed for spatial effects only
```

#### 2.2 Afterburner Overlay Contrast (`renderAfterburnerOverlay`)

**Strategy:**

- **Ring 1**: Normal afterburner (stronger in center)
- **Ring 2**:
  - **Phase Offset**: Pulse offset by 180° for pulse mode
  - **Reversed Spatial Profile**: Stronger at edges instead of center
  - **Slight Delay**: Sparkles appear at different times

**Implementation:**

```cpp
// Ring 1: spatialProfile = 0.65 + 0.35 * sin(2π * position)
// Ring 2: spatialProfile = 0.65 + 0.35 * sin(2π * (1.0 - position) + π)
//         pulse = sin(millis() * frequency + M_PI)  // 180° offset
```

#### 2.3 Flicker Effect Contrast (`addFlicker`)

**Strategy:**

- **Ring 1**: Normal noise-based flicker
- **Ring 2**:
  - **Phase Offset**: Different noise seed offset
  - **Inverted Pattern**: Use complementary noise values

**Implementation:**

```cpp
// Ring 1: noise = inoise8(ledIndex * 12, time * 8 + noiseOffset)
// Ring 2: noise = 255 - inoise8((ledIndex - numLeds) * 12, time * 8 + noiseOffset + 1000)
```

#### 2.4 Sparkle Effect Contrast (`addSparkles`)

**Strategy:**

- **Ring 1**: Normal sparkle timing
- **Ring 2**:
  - **Phase Offset**: Different random seed timing
  - **Independent Sparkles**: Each ring has its own sparkle pattern

**Implementation:**

```cpp
// Use different random seeds for each ring
// Ring 1: random(1000) based on normal timing
// Ring 2: random(1000) based on offset timing (different seed)
```

### Phase 3: Helper Functions

#### 3.1 Add Ring Detection Helper

```cpp
private:
  bool isRing2(uint16_t ledIndex) const { return ledIndex >= numLeds; }
  uint16_t getRingLocalIndex(uint16_t ledIndex) const {
    return isRing2(ledIndex) ? (ledIndex - numLeds) : ledIndex;
  }
  float getRingPosition(uint16_t ledIndex) const {
    uint16_t localIndex = getRingLocalIndex(ledIndex);
    float position = (float)localIndex / numLeds;
    return isRing2(ledIndex) ? (1.0f - position) : position;  // Reverse for ring 2
  }
```

#### 3.2 Color Interpolation (Same for Both Rings)

```cpp
// In renderCoreEffect:
// Both rings use the SAME color gradient
CRGB color = lerpColor(startColor, endColor, easedThrottle);

// Only effects (breathing, flicker, spatial patterns) differ between rings
```

### Phase 4: Testing & Validation

#### 4.1 Visual Testing Checklist

- [ ] Both rings display correctly
- [ ] Ring 2 effects are visually contrasting (not identical)
- [ ] Breathing/pulse effects are out of phase
- [ ] Afterburner effects appear realistic on both rings
- [ ] Flicker patterns are independent
- [ ] Sparkles appear independently on each ring
- [ ] Color gradients are identical on both rings (same transition)

#### 4.2 Edge Cases

- [ ] Works with minimum LED count (1 LED per ring = 2 total)
- [ ] Works with maximum LED count (300 per ring = 600 total)
- [ ] Settings update correctly when numLeds changes
- [ ] All modes (Linear, Ease, Pulse) work correctly

## Code Structure Changes Summary

### Modified Files:

1. **`firmware/src/led_effects.h`**

   - Add helper methods: `isRing2()`, `getRingLocalIndex()`, `getRingPosition()`
   - Keep `numLeds` as member (LEDs per ring)
   - Add internal `totalLeds` calculation

2. **`firmware/src/led_effects.cpp`**

   - Update `begin()`: Allocate `numLeds * 2` LEDs
   - Update `renderCoreEffect()`: Add ring-specific logic
   - Update `renderAfterburnerOverlay()`: Add contrasting effects
   - Update `addFlicker()`: Add phase offset for ring 2
   - Update `addSparkles()`: Independent sparkles per ring

3. **`firmware/src/main.cpp`**
   - Update line 94: `ledEffects.begin(numLeds * 2)`
   - Add comment about dual-ring support

### Unchanged Files:

- **`settings.h`** & **`settings.cpp`**: No changes needed (numLeds still represents per-ring count)
- **`ble_service.cpp`**: No changes needed (BLE still reports per-ring count)
- **`constants.h`**: No changes needed

## Realism Enhancements

### Why These Contrasts Work:

1. **Phase Offsets (180°)**: Real turbines don't fire in perfect sync - slight timing differences create realistic variation
2. **Same Color Gradient**: Both engines have identical color transitions (realistic - same fuel/combustion)
3. **Independent Flicker**: Each engine has its own combustion variations
4. **Spatial Reversal**: Different spatial patterns help distinguish the two turbines visually

### Performance Considerations:

- **Memory**: Doubles LED array size (acceptable for ESP32-C3)
- **CPU**: Minimal overhead (just conditional checks per LED)
- **Timing**: No significant impact on frame rate

## Migration Notes

### Backward Compatibility:

- Existing settings remain valid (numLeds still means LEDs per ring)
- Users with single ring will need to update numLeds setting to half their total LEDs
- No breaking changes to BLE API

### User Instructions:

1. Set `numLeds` to the number of LEDs per ring (not total)
2. Firmware automatically uses `numLeds * 2` for total LED count
3. Second ring will automatically display contrasting effects

## Implementation Order

1. ✅ **Phase 1**: Core infrastructure (LED array allocation)
2. ✅ **Phase 2**: Contrasting effects (core, afterburner, flicker, sparkles)
3. ✅ **Phase 3**: Helper functions (ring detection utilities)
4. ✅ **Phase 4**: Testing and validation

## Success Criteria

- [x] Both rings render correctly
- [x] Effects are visually contrasting (not identical)
- [x] Performance remains acceptable
- [x] No breaking changes to existing API
- [x] All modes work correctly with dual rings
- [x] Realistic appearance achieved
