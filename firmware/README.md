# ESP32 C3 Afterburner Project

A complete LED afterburner effect system for RC models, featuring real-time throttle input, customizable LED effects, Bluetooth control, OLED status display, and enhanced mobile app integration.

## 🚀 Features

### Core Functionality

- **Real-time Throttle Input**: PWM signal processing from RC receivers or potentiometers
- **Enhanced Throttle Calibration**: Multi-position validation with stability checks
- **Dynamic LED Effects**: WS2812B LED strip with afterburner simulation
- **Speed-Controlled Animations**: Adjustable timing (100-5000ms) for all effects
- **Multiple Effect Modes**: Linear, Ease, and Pulse animations
- **Bluetooth Control**: Remote configuration and real-time monitoring via BLE app
- **OLED Status Display**: Built-in 128x64 OLED with navigation
- **Settings Persistence**: Flash memory storage for configurations

### Enhanced Throttle Calibration

- **Multi-position validation**: Requires multiple visits to min/max positions
- **Stability checking**: Ensures readings are consistent before saving
- **Time-based validation**: Prevents immediate saving for better accuracy
- **Real-time progress**: Mobile app shows calibration progress with visit counts
- **Automatic completion detection**: App automatically detects when calibration is done

### LED Effects with Speed Control

- **Core Effect**: Simulates jet engine core with color gradients
- **Afterburner Overlay**: Dynamic flame effect based on throttle
- **Flicker Simulation**: Realistic engine flickering with adjustable speed
- **Sparkle Effects**: Random sparkles for authenticity with speed control
- **Color Customization**: Full RGB control for start/end colors
- **Speed Settings**: 100-5000ms range for animation timing control

### User Interface

- **Three-Page OLED Display**:
  - Main Status (mode, throttle, connection)
  - Settings (speed, brightness, LED count, threshold)
  - Detailed Status (colors, connection details)
- **Navigation Button**: Manual page control with debouncing
- **Serial Monitor**: Essential debug information (cleaned up)
- **BLE App**: Remote control, monitoring, and calibration

## 📋 Hardware Requirements

### Required Components

- **ESP32-C3 OLED Development Board** (main controller with built-in OLED)
- **0.42-inch OLED Display** (built-in, no external connections needed)
- **WS2812B LED Strip** (afterburner effect display)
- **Navigation Button** (momentary push button)
- **Throttle Input** (RC receiver or potentiometer)
- **Power Supply** (5V for LED strip, 3.3V for logic)

### Optional Components

- **External 5V Power Supply** (for large LED strips)
- **Capacitors** (for LED strip stability)
- **Heat Shrink/Electrical Tape** (for insulation)

## 🔧 Pin Configuration

### ESP32-C3 OLED Board Pin Assignments

| Pin    | Function    | Direction     | Component             |
| ------ | ----------- | ------------- | --------------------- |
| GPIO0  | BOOT Button | Input         | Programming           |
| GPIO1  | ADC1        | Input         | **Throttle Input**    |
| GPIO2  | ADC2        | Input         | **Navigation Button** |
| GPIO3  | ADC3        | Input         | Available             |
| GPIO4  | ADC4        | Input         | Available             |
| GPIO5  | I2C SDA     | Bidirectional | **OLED Display**      |
| GPIO6  | I2C SCL     | Output        | **OLED Display**      |
| GPIO7  | SS (SPI)    | Output        | Available             |
| GPIO8  | SDA (I2C)   | Bidirectional | Available             |
| GPIO9  | SCL (I2C)   | Output        | Available             |
| GPIO10 | Available   | -             | Available             |
| GPIO20 | UART RX     | Input         | Serial Communication  |
| GPIO21 | UART TX     | Output        | **WS2812B Strip**     |

### Pin Notes

- **GPIO5 & GPIO6**: Built-in OLED display (no external connections needed)
- **GPIO21**: LED data output
- **GPIO1**: Throttle input (ADC1)
- **GPIO2**: Navigation button (ADC2)
- **GPIO8 & GPIO9**: External I2C available for future expansion
- **GPIO3 & GPIO4**: Additional ADC inputs available

## 📚 Documentation

### Setup Guides

- **[Complete Wiring Diagram](ESP32_AFTERBURNER_COMPLETE_WIRING.md)** - Full hardware setup (updated for ESP32-C3 OLED board)
- **[OLED Display Guide](OLED_DISPLAY_README.md)** - Display functionality
- **[Navigation Button Guide](NAVIGATION_BUTTON_WIRING.md)** - Button setup
- **[Speed Setting Implementation](SPEED_SETTING_IMPLEMENTATION.md)** - Animation speed control documentation

### Code Structure

- **main.cpp** - Main application logic with calibration management
- **settings.h/cpp** - Configuration management and flash storage
- **throttle.h/cpp** - PWM input processing and enhanced calibration
- **led_effects.h/cpp** - LED animation system with speed control
- **ble_service.h/cpp** - Bluetooth communication and notifications
- **oled_display.h/cpp** - Display interface
- **constants.h** - System constants and calibration parameters

## 🛠️ Installation

### 1. Hardware Setup

```bash
# Follow the complete wiring guide
# Connect components according to pin configuration
# Ensure proper power supply for LED count
```

### 2. Software Setup

```bash
# Install PlatformIO
# Clone this repository
# Install dependencies (FastLED, NimBLE, U8g2)
```

### 3. Configuration

```cpp
// Edit main.cpp to configure:
oledDisplay.begin(2);  // Navigation button pin (GPIO2)
// LED count in settings
// Throttle input pin (default: GPIO1)
// LED data pin (default: GPIO21)
```

### 4. Upload

```bash
# Upload to ESP32-C3 OLED board via USB-C
# Monitor Serial output (115200 baud)
# Test all components
```

## 🎮 Usage

### Basic Operation

1. **Power On**: ESP32 boots and shows startup screen
2. **Throttle Input**: Connect RC receiver or potentiometer
3. **Calibration**: Use mobile app to calibrate throttle range
4. **LED Effects**: Watch afterburner simulation respond to throttle
5. **Navigation**: Use button to cycle through OLED pages
6. **BLE Control**: Connect phone app for remote configuration

### Enhanced Calibration Process

1. **Start Calibration**: Use mobile app to begin calibration
2. **Multiple Positions**: Move throttle to min/max positions multiple times
3. **Stability Check**: System ensures readings are consistent
4. **Progress Monitoring**: App shows visit counts and progress
5. **Automatic Completion**: System detects when calibration is complete
6. **Settings Saved**: Calibration values stored to flash memory

### Effect Modes

- **Linear**: Direct throttle-to-brightness mapping
- **Ease**: Smooth acceleration curve
- **Pulse**: Pulsing effect at high throttle with speed control

### Settings Control

- **Speed**: Animation timing (100-5000ms) for all effects
- **Brightness**: LED intensity (10-255)
- **LED Count**: Number of LEDs in strip
- **AB Threshold**: Afterburner activation point (0-100%)
- **Colors**: Start and end RGB values

## 🔍 Troubleshooting

### Common Issues

1. **OLED Display Issues**

   - Check I2C connections (GPIO4/5)
   - Verify power supply (3.3V)
   - Ensure correct screen type (128x64, starting at 13,14)

2. **LED Strip Problems**

   - Verify data connection (GPIO21)
   - Check power supply adequacy
   - Ensure correct data flow direction

3. **Throttle Input Issues**

   - Check PWM signal on GPIO1
   - Verify signal range (1-2ms typical)
   - Test with potentiometer
   - Ensure proper calibration with multiple position visits

4. **BLE Connection Problems**

   - Check antenna connection
   - Verify power supply stability
   - Look for "ABurner" device in app
   - Ensure proper permissions on mobile device

5. **Calibration Issues**
   - Move throttle to min/max positions multiple times
   - Ensure stable readings before saving
   - Check mobile app for calibration progress
   - Verify flash memory is working properly

### Debug Information

- **Serial Monitor**: Essential system status (cleaned up)
- **OLED Display**: Current settings and throttle
- **LED Patterns**: Visual system state indication
- **BLE App**: Remote monitoring, control, and calibration
- **Mobile App Logs**: Critical error information only

## 📊 Performance

### Power Requirements

- **ESP32 C3**: ~100-200mA at 5V
- **LED Strip**: ~60mA per LED at full brightness
- **45 LEDs**: ~2.7A at 5V (13.5W)
- **100 LEDs**: ~6A at 5V (30W)

### Timing

- **Main Loop**: 50 FPS (20ms delay)
- **OLED Update**: 500ms intervals
- **BLE Status**: 200ms notifications
- **LED Effects**: Real-time rendering with speed control
- **Calibration**: Multi-position validation with stability checks

## 🔮 Future Enhancements

### Planned Features

- **WiFi Connectivity**: Web interface for configuration
- **Multiple LED Strips**: Support for multiple afterburner zones
- **Sound Effects**: Audio simulation via buzzer
- **Temperature Monitoring**: Thermal protection
- **Preset Management**: Save/load effect configurations
- **Advanced Effects**: More realistic engine simulations
- **Calibration Profiles**: Multiple calibration presets

### Hardware Expansions

- **Additional Sensors**: Temperature, vibration, pressure
- **External Displays**: Larger status displays
- **Audio Output**: Engine sound simulation
- **Power Management**: Battery monitoring and protection

## 📄 License

This project is open source and available under the MIT License.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

### Code Style Guidelines

- Follow existing code formatting
- Add comments for complex logic
- Update documentation for new features
- Include tests when possible
- Keep debug logs minimal and essential

## 📞 Support

For support and questions:

1. Check the troubleshooting section
2. Review the wiring diagrams
3. Monitor Serial output for debug information
4. Check mobile app for calibration status
5. Open an issue with detailed problem description

---

**Happy Flying! 🛩️**
