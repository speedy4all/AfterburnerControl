# OLED Screen Specifications for ESP32-C3 Afterburner

## ⚠️ IMPORTANT WARNING ⚠️

**This firmware is specifically designed for a particular 128x64 OLED display with specific offset characteristics. It is NOT compatible with standard 0.42-inch screens and cannot be directly replaced.**

## Screen Specifications

- **Display Type**: 128x64 OLED (SSD1306 compatible)
- **Starting Point Offset**: X=13, Y=14 pixels
- **Resolution**: 128x64 pixels
- **Interface**: I2C (Hardware)
- **I2C Pins**: SDA=GPIO5, SCL=GPIO6

## Why This Screen is Different

Most standard 0.42-inch OLED screens start drawing at position (0, 0), but this specific screen has an offset starting point at (13, 14). This means:

- Text and graphics positioned at (0, 0) on a standard screen will appear at (13, 14) on this screen
- The actual usable area is effectively smaller due to the offset
- Standard OLED libraries may not work correctly without offset compensation

## Code Adaptations

The firmware has been specifically adapted with:

1. **Offset Constants**:

   ```cpp
   static const uint8_t SCREEN_OFFSET_X = 13;
   static const uint8_t SCREEN_OFFSET_Y = 14;
   ```

2. **Offset Helper Functions**:

   ```cpp
   uint8_t getOffsetX(uint8_t x) const { return x + SCREEN_OFFSET_X; }
   uint8_t getOffsetY(uint8_t y) const { return y + SCREEN_OFFSET_Y; }
   ```

3. **All Drawing Operations**: Use these offset functions to ensure proper positioning

## Compatibility Issues

### ❌ NOT Compatible With:

- Standard 0.42-inch OLED screens
- Screens without offset compensation
- Different resolution displays
- Different I2C pin configurations

### ✅ Compatible With:

- This specific 128x64 OLED screen with (13, 14) offset
- ESP32-C3 boards with GPIO5/GPIO6 I2C configuration

## Purchasing Considerations

**BEFORE BUYING A REPLACEMENT SCREEN:**

1. **Verify the exact model number** matches this specification
2. **Check if it has the same offset characteristics** (13, 14)
3. **Confirm the I2C pin configuration** is compatible
4. **Verify with a simple Arduino sketch** to verify offset behavior

## Alternative Solutions

If you need to use a different screen:

1. **Modify the offset constants** in `oled_display.h`
2. **Adjust all drawing coordinates** throughout the code
3. **Verify thoroughly** with the new screen
4. **Update this documentation** with new specifications

## Verification

To verify your screen is compatible:

1. Upload the firmware
2. Check if text appears in the correct positions
3. Verify all three pages display properly
4. Ensure the throttle bar and other graphics are visible

## Support

If you encounter display issues:

1. Check the serial monitor for error messages
2. Verify I2C connection and pin assignments
3. Confirm screen specifications match exactly
4. Consider using a compatible screen instead of modifying code

---

**Remember: This is NOT a standard OLED screen. Use only the exact model specified or be prepared to modify the code extensively.**
