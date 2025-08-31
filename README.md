# ESP8266 Afterburner Controller - NodeMCU V3 Edition

A sophisticated LED afterburner controller for RC models using ESP8266 NodeMCU V3, featuring automatic throttle calibration, real-time LED effects, and wireless control via React Native app.

![ESP8266 Afterburner](https://img.shields.io/badge/ESP8266-NodeMCU%20V3-blue)
![Version](https://img.shields.io/badge/Version-2.0.0-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

## 🚀 Features

### ✨ Core Features

- **🎯 Automatic Throttle Calibration**: Works with any transmitter (Futaba, Spektrum, FlySky, etc.)
- **💡 Real-time LED Effects**: Dynamic afterburner effects that respond to throttle input
- **📱 Wireless Control**: React Native app for real-time settings adjustment
- **🎨 Multiple Effect Modes**: Static, Pulse, and Afterburner modes
- **🔗 WebSocket Communication**: Real-time bidirectional communication
- **💾 EEPROM Settings Storage**: Persistent configuration across reboots

### 🔧 Hardware Features

- **ESP8266 NodeMCU V3** compatibility
- **WS2812B LED Strip** support (up to 300 LEDs)
- **PWM Throttle Input** (automatic calibration)
- **5V Power Supply** support for LED strips
- **WiFi Access Point** for wireless control

## 📋 Quick Start

### 1. Hardware Setup

```
ESP8266 NodeMCU V3:
├── D5 (GPIO14) → Throttle PWM Input
├── D6 (GPIO12) → LED Strip Data (Din)
├── 5V → LED Strip Power (external supply)
└── GND → Common Ground (connect all GNDs)
```

### 2. Firmware Upload

```bash
cd firmware
pio run --target upload
```

### 3. First Time Setup

1. **Power on** the ESP8266
2. **Move throttle stick** from MIN to MAX several times (automatic calibration)
3. **Connect to WiFi** "Afterburner_AP" (password: afterburner123)
4. **Launch mobile app** and start controlling!

## 🎮 Usage

### Automatic Calibration

The system automatically detects your transmitter's PWM range:

```
=== THROTTLE CALIBRATION STARTED ===
Move your throttle stick from MIN to MAX several times
Calibration: 10 samples, Min: 988 us, Max: 2011 us
...
=== THROTTLE CALIBRATION COMPLETE ===
Min Pulse: 988 us, Max Pulse: 2011 us
Calibration saved!
```

### LED Effects

- **Mode 0: Static** - Solid color with throttle-responsive brightness
- **Mode 1: Pulse** - Smooth pulsing effect
- **Mode 2: Afterburner** - Dynamic flame effect with sparkles

### Mobile App Controls

- **Mode Selection**: Choose effect type
- **Color Control**: Set start/end colors (RGB)
- **Brightness**: Adjust LED intensity (10-255)
- **Speed**: Control animation speed (100-5000ms)
- **LED Count**: Set number of LEDs (1-300)
- **Afterburner Threshold**: Set activation point (0-100%)

## 🔧 Configuration

### Pin Definitions

```cpp
#define THR_PIN 14  // D5 - GPIO14 (Throttle input)
#define LED_PIN 12  // D6 - GPIO12 (LED strip data)
```

### WiFi Settings

```cpp
#define WIFI_SSID "Afterburner_AP"
#define WIFI_PASSWORD "afterburner123"
#define WEB_SOCKET_PORT 81
```

## 🐛 Troubleshooting

### Common Issues

**Throttle not responding?**

- Check wiring to D5 (GPIO14)
- Ensure transmitter is powered on
- Move throttle during calibration

**LEDs not lighting up?**

- Check 5V power supply connection
- Verify data wire to D6 (GPIO12)
- Ensure common ground connection

**Can't connect to WiFi?**

- Check SSID "Afterburner_AP"
- Verify password "afterburner123"
- Ensure device is within range

## 📊 Debug Output

The system provides comprehensive debug information:

```
ESP8266 Afterburner Starting...
=== THROTTLE CALIBRATION STARTED ===
Calibration: 10 samples, Min: 988 us, Max: 2011 us
=== THROTTLE CALIBRATION COMPLETE ===
ESP8266 Afterburner Ready!
Throttle: 0.07, Mode: 1, LEDs: 19, Calibrated: 988-2011 us
PWM: 988 us -> 0.07 throttle
LED Colors - Start: [255,0,0], End: [0,255,0], Throttle: 0.07
```

## 🔄 Recent Changes (v2.0)

### ✅ Added Features

- **Automatic Throttle Calibration**: Works with any transmitter
- **Enhanced PWM Detection**: Improved signal validation
- **Real-time Debug Output**: Comprehensive serial monitoring
- **Dynamic Color Mapping**: Uses calibrated PWM range
- **Improved Error Handling**: Better failsafe mechanisms

### ❌ Removed Features

- **OLED Display Support**: Removed to reduce complexity
- **Manual PWM Configuration**: Replaced with auto-calibration
- **Excessive Debug Output**: Streamlined for clarity

### 🔧 Technical Improvements

- **Faster Throttle Response**: Increased smoothing factor
- **Better Signal Validation**: Improved PWM range detection
- **Enhanced LED Effects**: More responsive color transitions
- **Optimized Memory Usage**: Reduced RAM footprint

## 📁 Project Structure

```
Afterburner/
├── firmware/                 # ESP8266 firmware
│   ├── src/                 # Source code
│   │   ├── main.cpp         # Main application
│   │   ├── throttle.h/cpp   # Throttle processing
│   │   ├── led_effects.h/cpp # LED animations
│   │   ├── settings.h/cpp   # Configuration
│   │   └── wifi_service.h/cpp # WiFi/WebSocket
│   ├── platformio.ini       # PlatformIO config
│   └── README.md            # Firmware documentation
├── AfterburnerControl/       # React Native app
│   ├── src/                 # App source code
│   ├── package.json         # Dependencies
│   └── README.md            # App documentation
├── docs/                    # Additional documentation
└── README.md               # This file
```

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **FastLED Library**: For excellent LED strip support
- **ArduinoJson**: For robust JSON handling
- **WebSockets**: For real-time communication
- **ESP8266 Community**: For continuous improvements

## 📞 Support

For support and questions:

1. Check the [firmware documentation](firmware/README.md)
2. Review the [troubleshooting section](firmware/README.md#troubleshooting)
3. Monitor Serial output for debug information
4. Open an issue with detailed problem description

---

**NodeMCU V3 Edition v2.0.0** - The most user-friendly and reliable ESP8266 afterburner controller yet! 🛩️
