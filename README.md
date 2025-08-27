# ESP32 C3 Afterburner Project

A complete LED afterburner effect system for RC models, featuring real-time throttle input, customizable LED effects, Bluetooth control, and OLED status display.

![ESP32 C3 Afterburner](https://img.shields.io/badge/ESP32%20C3%20Afterburner-Project-blue)
![Platform](https://img.shields.io/badge/Platform-ESP32%20C3-green)
![License](https://img.shields.io/badge/License-MIT-yellow)
![Firmware Build](https://github.com/speedy4all/AfterburnerControl/workflows/Build%20ESP32%20C3%20Firmware/badge.svg)
![React Native Build](https://github.com/speedy4all/AfterburnerControl/workflows/Build%20React%20Native%20App/badge.svg)
![Tests](https://github.com/speedy4all/AfterburnerControl/workflows/Run%20Tests/badge.svg)

## 🚀 Project Overview

This project creates a realistic afterburner effect for RC jet models using an ESP32 C3 OLED development board. The system features:

- **Real-time throttle input processing** from RC receivers or potentiometers
- **Dynamic LED effects** with WS2812B strips simulating jet engine afterburner
- **Bluetooth remote control** via BLE for real-time configuration
- **OLED status display** with three-page interface and navigation
- **Multiple effect modes** (Linear, Ease, Pulse) for different engine characteristics
- **Settings persistence** with EEPROM storage

## 📁 Project Structure

```
ESP32-C3-Afterburner/
├── firmware/                          # ESP32 C3 firmware
│   ├── src/                          # Source code
│   │   ├── main.cpp                  # Main application
│   │   ├── settings.h/cpp            # Settings management
│   │   ├── throttle.h/cpp            # PWM input processing
│   │   ├── led_effects.h/cpp         # LED animation system
│   │   ├── ble_service.h/cpp         # Bluetooth communication
│   │   └── oled_display.h/cpp        # OLED display interface
│   ├── platformio.ini                # PlatformIO configuration
│   ├── README.md                     # Firmware documentation
│   ├── ESP32_AFTERBURNER_COMPLETE_WIRING.md
│   ├── OLED_DISPLAY_README.md
│   ├── NAVIGATION_BUTTON_WIRING.md
│   └── PROJECT_VERIFICATION.md
├── AfterburnerControl/               # React Native mobile app
│   ├── src/                          # App source code
│   ├── android/                      # Android specific files
│   ├── ios/                          # iOS specific files
│   └── package.json                  # Dependencies
├── docs/                             # Additional documentation
├── .gitignore                        # Git ignore rules
└── README.md                         # This file
```

## 🛠️ Hardware Requirements

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

- **[Complete Wiring Diagram](firmware/ESP32_AFTERBURNER_COMPLETE_WIRING.md)** - Full hardware setup
- **[OLED Display Guide](firmware/OLED_DISPLAY_README.md)** - Display functionality
- **[Navigation Button Guide](firmware/NAVIGATION_BUTTON_WIRING.md)** - Button setup
- **[Project Verification](firmware/PROJECT_VERIFICATION.md)** - Complete verification checklist

### Firmware Documentation

- **[Firmware README](firmware/README.md)** - Detailed firmware documentation
- **Code Structure** - Modular design with separate classes for each component
- **API Reference** - Function descriptions and parameters

## 🚀 Quick Start

### 1. Hardware Setup

```bash
# Follow the complete wiring guide
# Connect components according to pin configuration
# Ensure proper power supply for LED count
```

### 2. Software Setup

```bash
# Install PlatformIO
pip install platformio

# Clone this repository
git clone https://github.com/speedy4all/AfterburnerControl.git
cd AfterburnerControl

# Install dependencies
cd firmware
pio lib install
```

### 3. Configuration

```cpp
// Edit firmware/src/main.cpp to configure:
oledDisplay.begin(2);  // Navigation button pin
// LED count in settings
// Throttle input pin (default: GPIO34)
```

### 4. Upload and Test

```bash
# Upload to ESP32 C3 via USB-C
pio run --target upload

# Monitor Serial output (115200 baud)
pio device monitor
```

## 🎮 Features

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

### Effect Modes

- **Linear**: Direct throttle-to-brightness mapping
- **Ease**: Smooth acceleration curve
- **Pulse**: Pulsing effect at high throttle

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

## 🤝 Contributing

We welcome contributions! Please feel free to submit pull requests or open issues for bugs and feature requests.

### Development Setup

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Code Style

- Follow existing code formatting
- Add comments for complex logic
- Update documentation for new features
- Include tests when possible

## 🔄 CI/CD Pipeline

This project uses GitHub Actions for continuous integration and deployment:

### Build Workflows

- **Firmware Build**: Automatically builds the ESP32 C3 firmware using PlatformIO
- **React Native Build**: Builds both Android APK and iOS app
- **Tests**: Runs unit tests for both firmware and mobile app

### Build Status

The build status badges above show the current state of:

- ✅ **Firmware Build**: ESP32 C3 firmware compilation
- ✅ **React Native Build**: Mobile app builds for Android and iOS
- ✅ **Tests**: Unit test execution and coverage

### Artifacts

Build artifacts are automatically generated and can be downloaded from the GitHub Actions page:

- Firmware binaries (.bin files)
- Android APK files
- iOS build packages
- Test coverage reports

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **ESP32 Community** for excellent documentation and support
- **FastLED Library** for powerful LED control capabilities
- **NimBLE Library** for efficient Bluetooth communication
- **U8g2 Library** for OLED display support
- **PlatformIO** for excellent development environment

## 📞 Support

For support and questions:

1. Check the troubleshooting section
2. Review the wiring diagrams
3. Monitor Serial output for debug information
4. Open an issue with detailed problem description

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

---

**Happy Flying! 🛩️**

---

<div align="center">

**ESP32 C3 Afterburner Project** - Bringing realistic afterburner effects to RC models

[![GitHub stars](https://img.shields.io/github/stars/speedy4all/AfterburnerControl?style=social)](https://github.com/speedy4all/AfterburnerControl/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/speedy4all/AfterburnerControl?style=social)](https://github.com/speedy4all/AfterburnerControl/network/members)
[![GitHub issues](https://img.shields.io/github/issues/speedy4all/AfterburnerControl)](https://github.com/speedy4all/AfterburnerControl/issues)
[![GitHub license](https://img.shields.io/github/license/speedy4all/AfterburnerControl)](https://github.com/speedy4all/AfterburnerControl/blob/master/LICENSE)

</div>
