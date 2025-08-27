# ESP32 C3 Afterburner Project

A complete LED afterburner effect system for RC models, featuring real-time throttle input, customizable LED effects, Bluetooth control, and OLED status display.

## 🚀 Features

### Core Functionality

- **Real-time Throttle Input**: PWM signal processing from RC receivers or potentiometers
- **Dynamic LED Effects**: WS2812B LED strip with afterburner simulation
- **Multiple Effect Modes**: Linear, Ease, and Pulse animations
- **Bluetooth Control**: Remote configuration via BLE app
- **OLED Status Display**: Built-in 128x64 OLED with navigation
- **Settings Persistence**: EEPROM storage for configurations

### LED Effects

- **Core Effect**: Simulates jet engine core with color gradients
- **Afterburner Overlay**: Dynamic flame effect based on throttle
- **Flicker Simulation**: Realistic engine flickering
- **Sparkle Effects**: Random sparkles for authenticity
- **Color Customization**: Full RGB control for start/end colors

### User Interface

- **Three-Page OLED Display**:
  - Main Status (mode, throttle, connection)
  - Settings (speed, brightness, LED count, threshold)
  - Detailed Status (colors, connection details)
- **Navigation Button**: Manual page control with debouncing
- **Serial Monitor**: Comprehensive debug information
- **BLE App**: Remote control and monitoring

## 📋 Hardware Requirements

### Required Components

- **ESP32 C3 OLED Development Board** (main controller)
- **WS2812B LED Strip** (afterburner effect display)
- **Navigation Button** (momentary push button)
- **Throttle Input** (RC receiver or potentiometer)
- **Power Supply** (5V for LED strip, 3.3V for logic)

### Optional Components

- **External 5V Power Supply** (for large LED strips)
- **Capacitors** (for LED strip stability)
- **Heat Shrink/Electrical Tape** (for insulation)

## 🔧 Pin Configuration

| Pin    | Function          | Direction     | Component            |
| ------ | ----------------- | ------------- | -------------------- |
| GPIO0  | BOOT Button       | Input         | Programming          |
| GPIO1  | UART TX           | Output        | Serial Communication |
| GPIO2  | Navigation Button | Input         | OLED Page Control    |
| GPIO3  | UART RX           | Input         | Serial Communication |
| GPIO4  | I2C SDA           | Bidirectional | OLED Display         |
| GPIO5  | I2C SCL           | Output        | OLED Display         |
| GPIO18 | LED Data          | Output        | WS2812B Strip        |
| GPIO34 | Throttle Input    | Input         | PWM Signal           |

## 📚 Documentation

### Setup Guides

- **[Complete Wiring Diagram](ESP32_AFTERBURNER_COMPLETE_WIRING.md)** - Full hardware setup
- **[OLED Display Guide](OLED_DISPLAY_README.md)** - Display functionality
- **[Navigation Button Guide](NAVIGATION_BUTTON_WIRING.md)** - Button setup

### Code Structure

- **main.cpp** - Main application logic
- **settings.h/cpp** - Configuration management
- **throttle.h/cpp** - PWM input processing
- **led_effects.h/cpp** - LED animation system
- **ble_service.h/cpp** - Bluetooth communication
- **oled_display.h/cpp** - Display interface

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
oledDisplay.begin(2);  // Navigation button pin
// LED count in settings
// Throttle input pin (default: GPIO34)
```

### 4. Upload

```bash
# Upload to ESP32 C3 via USB-C
# Monitor Serial output (115200 baud)
# Test all components
```

## 🎮 Usage

### Basic Operation

1. **Power On**: ESP32 boots and shows startup screen
2. **Throttle Input**: Connect RC receiver or potentiometer
3. **LED Effects**: Watch afterburner simulation respond to throttle
4. **Navigation**: Use button to cycle through OLED pages
5. **BLE Control**: Connect phone app for remote configuration

### Effect Modes

- **Linear**: Direct throttle-to-brightness mapping
- **Ease**: Smooth acceleration curve
- **Pulse**: Pulsing effect at high throttle

### Settings Control

- **Speed**: Animation timing (100-5000ms)
- **Brightness**: LED intensity (10-255)
- **LED Count**: Number of LEDs in strip
- **AB Threshold**: Afterburner activation point
- **Colors**: Start and end RGB values

## 🔍 Troubleshooting

### Common Issues

1. **OLED Display Issues**

   - Check I2C connections (GPIO4/5)
   - Verify power supply (3.3V)
   - Ensure correct screen type (128x64, starting at 13,14)

2. **LED Strip Problems**

   - Verify data connection (GPIO18)
   - Check power supply adequacy
   - Ensure correct data flow direction

3. **Throttle Input Issues**

   - Check PWM signal on GPIO34
   - Verify signal range (1-2ms typical)
   - Test with potentiometer

4. **BLE Connection Problems**
   - Check antenna connection
   - Verify power supply stability
   - Look for "ABurner" device in app

### Debug Information

- **Serial Monitor**: Real-time system status
- **OLED Display**: Current settings and throttle
- **LED Patterns**: Visual system state indication
- **BLE App**: Remote monitoring and control

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
- **LED Effects**: Real-time rendering

## 🔮 Future Enhancements

### Planned Features

- **WiFi Connectivity**: Web interface for configuration
- **Multiple LED Strips**: Support for multiple afterburner zones
- **Sound Effects**: Audio simulation via buzzer
- **Temperature Monitoring**: Thermal protection
- **Preset Management**: Save/load effect configurations
- **Advanced Effects**: More realistic engine simulations

### Hardware Expansions

- **Additional Sensors**: Temperature, vibration, pressure
- **External Displays**: Larger status displays
- **Audio Output**: Engine sound simulation
- **Power Management**: Battery monitoring and protection

## 📄 License

This project is open source and available under the MIT License.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## 📞 Support

For support and questions:

1. Check the troubleshooting section
2. Review the wiring diagrams
3. Monitor Serial output for debug information
4. Open an issue with detailed problem description

---

**Happy Flying! 🛩️**
