# ESP8266 Afterburner Controller - NodeMCU V3 Edition

A sophisticated LED afterburner controller for RC models using ESP8266 NodeMCU V3, featuring automatic throttle calibration, real-time LED effects, and wireless control via React Native app.

![ESP8266 Afterburner](https://img.shields.io/badge/ESP8266-NodeMCU%20V3-blue)
![Version](https://img.shields.io/badge/Version-2.1.0-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

## 🚀 Features

### ✨ Core Features

- **🎯 Automatic Throttle Calibration**: Works with any transmitter (Futaba, Spektrum, FlySky, etc.)
- **💡 Real-time LED Effects**: Dynamic afterburner effects that respond to throttle input
- **📱 Wireless Control**: React Native app for real-time settings adjustment
- **🎨 Multiple Effect Modes**: Static, Pulse, and Afterburner modes
- **🔗 WebSocket Communication**: Real-time bidirectional communication
- **💾 EEPROM Settings Storage**: Persistent configuration across reboots
- **🛡️ Reliable Communication**: Optimized to prevent controller crashes

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

**🎯 Simplified Interface**: The app now uses a "Push All Settings" approach for maximum reliability:

- **Mode Selection**: Choose effect type
- **Color Control**: Set start/end colors (RGB) with visual color picker
- **Brightness**: Adjust LED intensity (10-255)
- **Speed**: Control animation speed (100-5000ms)
- **LED Count**: Set number of LEDs (1-300)
- **Afterburner Threshold**: Set activation point (0-100%)
- **🚀 Push All Settings**: Send all changes at once to prevent controller crashes

**💡 How to Use**:

1. Adjust any settings using the intuitive controls
2. Tap "🚀 Push All Settings to Device" to send all changes
3. The device will update immediately without crashes

### Throttle Calibration Controls

- **Start Calibration**: Begin automatic throttle calibration
- **Reset to Default Values**: Use default PWM range (900-2100 μs)

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

**Controller crashes when changing settings?**

- Use "Push All Settings" instead of individual controls
- Ensure you're connected to the Afterburner_AP WiFi network
- Check that the ESP8266 is powered properly

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

## 🔄 Recent Changes (v2.1.0)

### ✅ Added Features

- **Simplified Mobile Interface**: Removed individual setting controls for reliability
- **"Push All Settings" Approach**: Send complete settings at once to prevent crashes
- **Enhanced Color Picker**: Visual RGB color selection with predefined palettes
- **Improved Error Handling**: Better validation and crash prevention
- **Streamlined UI**: Cleaner, more intuitive interface

### ❌ Removed Features

- **Individual Setting Controls**: Removed to prevent controller crashes
- **Test Ping/Connection**: Removed debugging features for cleaner interface
- **OLED Display Support**: Removed to reduce complexity
- **Manual PWM Configuration**: Replaced with auto-calibration

### 🔧 Technical Improvements

- **Atomic Settings Updates**: Complete settings replacement instead of partial updates
- **Enhanced Validation**: Comprehensive input validation on both app and firmware
- **Optimized WebSocket Communication**: Reduced message frequency and improved reliability
- **Better Memory Management**: Reduced JSON buffer sizes and improved EEPROM handling
- **Deferred EEPROM Saves**: Moved saves to main loop to prevent watchdog resets

### 🛡️ Reliability Improvements

- **Crash Prevention**: Multiple layers of validation and error handling
- **Watchdog Timer Management**: Proper feeding during critical operations
- **Rate Limiting**: Prevents overwhelming the controller with rapid updates
- **Memory Optimization**: Reduced memory fragmentation and improved stability

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
│   │   ├── screens/         # UI screens
│   │   ├── components/      # Reusable components
│   │   └── websocket/       # WebSocket communication
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

**NodeMCU V3 Edition v2.1.0** - The most reliable and user-friendly ESP8266 afterburner controller! 🛩️
