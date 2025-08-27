# ESP32 C3 OLED Display Integration

This document describes the OLED display functionality added to the ESP32 C3 Afterburner project.

## Hardware Setup

The ESP32 C3 OLED development board comes with a built-in 0.42-inch OLED display that uses I2C communication. **IMPORTANT**: This screen is different from other 0.42-inch screens. The starting point is at (13, 14) and it's a 128x64 pixel display.

### Pin Connections

- **SDA**: GPIO4 (default I2C data pin)
- **SCL**: GPIO5 (default I2C clock pin)
- **VCC**: 3.3V
- **GND**: Ground

### Screen Specifications

- **Resolution**: 128x64 pixels
- **Starting Point**: (13, 14) - this is crucial for proper display
- **Interface**: I2C
- **Size**: 0.42-inch
- **Note**: This is a custom screen configuration, not compatible with standard 0.42-inch OLED replacements

## Display Features

The OLED display shows three different pages of information that automatically cycle every 3 seconds, or can be manually navigated using the BOOT button.

### Page 1: Main Status

- **Title**: "Afterburner"
- **Mode**: Current LED effect mode (Linear, Ease, or Pulse)
- **Throttle**: Current throttle percentage with visual bar
- **BLE Status**: Connection status (ON/OFF)
- **Page Indicator**: "Page 1/3"

### Page 2: Settings

- **Title**: "Settings"
- **Speed**: Animation speed in milliseconds or seconds
- **Brightness**: LED brightness level (10-255)
- **LEDs**: Number of connected LEDs
- **AB Threshold**: Afterburner threshold percentage
- **Page Indicator**: "Page 2/3"

### Page 3: Detailed Status

- **Title**: "Status"
- **Start Color**: RGB values for start color
- **End Color**: RGB values for end color
- **BLE Connection**: Detailed connection status
- **Page Indicator**: "Page 3/3"

## Navigation

### Automatic Navigation

- Pages automatically advance every 3 seconds
- Continuous cycling through all three pages

### Manual Navigation

- **Dedicated Navigation Button** (GPIO2): Press to advance to next page
- Button is debounced to prevent multiple triggers
- Can be used to stay on a specific page longer
- Auto-advance is disabled when button is configured
- Serial output provides feedback when button is pressed

## Technical Details

### Libraries Used

- **U8g2**: Universal graphics library for monochrome displays
- **Wire**: I2C communication library

### Display Configuration

- **Display Type**: SSD1306 128x64 NONAME (custom configuration)
- **Interface**: Hardware I2C
- **Rotation**: 0 degrees (normal orientation)
- **Font**: 6x10 pixel font for optimal readability
- **Starting Point**: (13, 14) - all text positioning accounts for this offset

### Update Frequency

- Display updates every 500ms by default
- Can be adjusted via `setUpdateInterval()` method
- Throttle bar updates in real-time

## Code Structure

### Files Added

- `oled_display.h`: Header file with class definition
- `oled_display.cpp`: Implementation file with all display methods
- `OLED_DISPLAY_README.md`: This documentation file

### Integration Points

- **main.cpp**: Initializes and updates the display
- **ble_service.h/cpp**: Provides connection status
- **settings.h**: Provides current settings data

### Key Methods

- `begin()`: Initialize display and show startup message
- `update()`: Main update method called from main loop
- `nextPage()`: Manually advance to next page
- `setPage()`: Jump to specific page
- `enable()`: Enable/disable display updates

## Troubleshooting

### Display Not Working

1. Check I2C connections (SDA=GPIO4, SCL=GPIO5)
2. Verify power supply (3.3V)
3. Check Serial Monitor for initialization messages
4. Ensure U8g2 library is installed

### Display Shows Garbage

1. Check I2C address (default should work)
2. Verify display type matches SSD1306 128x64
3. Check for power supply issues
4. **Important**: This is a custom screen with starting point at (13, 14) - standard 0.42-inch OLED replacements will not work

### Button Not Working

1. Verify navigation button is connected to GPIO2 (or configured pin)
2. Check for proper wiring (button to GPIO2 and GND)
3. Monitor Serial output for button press events
4. Ensure button is momentary (not latching)
5. Check Serial Monitor for "Navigation button initialized on GPIO2" message

## Future Enhancements

Potential improvements for the OLED display:

- Add WiFi status display
- Show system uptime
- Display temperature sensor data
- Add configuration menu
- Show preset names
- Add battery level indicator (if applicable)
- Add long-press functionality for different actions
- Add multiple button support for different functions
- Add button LED indicator for visual feedback
