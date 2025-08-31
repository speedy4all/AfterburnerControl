# ESP8266 Afterburner Controller - NodeMCU V3 Edition

A sophisticated LED afterburner controller for RC models using ESP8266 NodeMCU V3, featuring automatic throttle calibration, real-time LED effects, and wireless control via React Native app.

## 🚀 Features

### Core Features

- **Automatic Throttle Calibration**: Automatically detects your transmitter's PWM range (works with any transmitter)
- **Real-time LED Effects**: Dynamic afterburner effects that respond to throttle input
- **Wireless Control**: React Native app for real-time settings adjustment
- **Multiple Effect Modes**: Static, Pulse, and Afterburner modes
- **WebSocket Communication**: Real-time bidirectional communication
- **EEPROM Settings Storage**: Persistent configuration across reboots

### Hardware Features

- **ESP8266 NodeMCU V3** compatibility
- **WS2812B LED Strip** support (up to 300 LEDs)
- **PWM Throttle Input** (automatic calibration)
- **5V Power Supply** support for LED strips
- **WiFi Access Point** for wireless control

## 📋 Requirements

### Hardware

- **ESP8266 NodeMCU V3** (or compatible)
- **WS2812B LED Strip** (5V)
- **5V Power Supply** (1A+ for LED strip)
- **RC Transmitter** (any brand - Futaba, Spektrum, FlySky, etc.)
- **Jumper Wires**

### Software

- **PlatformIO** or **Arduino IDE**
- **React Native** (for mobile app)
- **FastLED Library** (v3.5.0+)
- **ArduinoJson Library** (v6.0+)
- **WebSockets Library** (v2.3.5+)

## 🔌 Wiring Diagram

### NodeMCU V3 Pinout

```
ESP8266 NodeMCU V3:
├── D5 (GPIO14) → Throttle PWM Input
├── D6 (GPIO12) → LED Strip Data (Din)
├── 3.3V → Logic level (ESP8266)
├── 5V → LED Strip Power (external supply)
├── GND → Common Ground (connect all GNDs)
└── D1/D2 → I2C (if using OLED display)
```

### Power Supply

- **LED Strip**: 5V external power supply (1A+ recommended)
- **ESP8266**: USB or 3.3V supply
- **Common Ground**: Connect ESP8266 GND to power supply GND

## 🛠️ Installation

### 1. Firmware Setup

#### Using PlatformIO (Recommended)

```bash
# Clone the repository
git clone <repository-url>
cd Afterburner/firmware

# Install dependencies
pio lib install "FastLED"
pio lib install "ArduinoJson"
pio lib install "WebSockets"

# Build and upload
pio run --target upload
```

#### Using Arduino IDE

1. Install required libraries:
   - FastLED
   - ArduinoJson
   - WebSockets
2. Open `firmware/src/main.cpp`
3. Select "NodeMCU 1.0" board
4. Upload the sketch

### 2. Mobile App Setup

```bash
cd Afterburner/AfterburnerControl
npm install
npx react-native run-android  # or run-ios
```

## 🎮 Usage

### First Time Setup

1. **Power on the ESP8266**
2. **Automatic Calibration**: The device will automatically start throttle calibration
3. **Move Throttle**: Move your throttle stick from MIN to MAX several times
4. **Wait for Completion**: Calibration completes after 100 samples (~5-10 seconds)
5. **Connect to WiFi**: Connect to "Afterburner_AP" network
6. **Open App**: Launch the React Native app

### Throttle Calibration

The system automatically calibrates your transmitter's PWM range:

```
=== THROTTLE CALIBRATION STARTED ===
Move your throttle stick from MIN to MAX several times
Calibration will complete automatically after 100 samples
Calibration: 10 samples, Min: 988 us, Max: 2011 us
Calibration: 20 samples, Min: 988 us, Max: 2011 us
...
=== THROTTLE CALIBRATION COMPLETE ===
Min Pulse: 988 us
Max Pulse: 2011 us
Range: 1023 us
Calibration saved!
```

### LED Effects

#### Mode 0: Static

- Solid color that changes brightness with throttle

#### Mode 1: Pulse

- Smooth pulsing effect with throttle-responsive intensity

#### Mode 2: Afterburner

- Dynamic afterburner effect with sparkles and color transitions

### Mobile App Controls

- **Mode Selection**: Choose between Static, Pulse, and Afterburner
- **Color Control**: Set start and end colors (RGB)
- **Brightness**: Adjust LED brightness (10-255)
- **Speed**: Control animation speed (100-5000ms)
- **LED Count**: Set number of LEDs (1-300)
- **Afterburner Threshold**: Set afterburner activation point (0-100%)

## 🔧 Configuration

### Pin Definitions

```cpp
// Throttle input
#define THR_PIN 14  // D5 - GPIO14

// LED strip
#define LED_PIN 12  // D6 - GPIO12

// WiFi settings
#define WIFI_SSID "Afterburner_AP"
#define WIFI_PASSWORD "afterburner123"
#define WEB_SERVER_PORT 80
#define WEB_SOCKET_PORT 81
```

### Calibration Settings

```cpp
#define CALIBRATION_SAMPLES 100  // Number of samples for calibration
#define PWM_TIMEOUT 50000        // PWM read timeout (50ms)
```

## 🐛 Troubleshooting

### Throttle Issues

**Problem**: Throttle not responding

- **Solution**: Check wiring to D5 (GPIO14)
- **Solution**: Ensure transmitter is powered on
- **Solution**: Move throttle during calibration

**Problem**: Throttle jumping between values

- **Solution**: Check for loose connections
- **Solution**: Add 0.1μF capacitor between signal and ground
- **Solution**: Verify power supply stability

### LED Issues

**Problem**: LEDs not lighting up

- **Solution**: Check 5V power supply connection
- **Solution**: Verify data wire connection to D6 (GPIO12)
- **Solution**: Ensure common ground connection
- **Solution**: Check LED strip type (must be WS2812B)

**Problem**: Wrong colors

- **Solution**: Check LED strip color order (GRB vs RGB)
- **Solution**: Verify power supply current rating

### WiFi Issues

**Problem**: Can't connect to WiFi

- **Solution**: Check SSID "Afterburner_AP"
- **Solution**: Verify password "afterburner123"
- **Solution**: Ensure device is within range

**Problem**: App not connecting

- **Solution**: Check WebSocket port 81
- **Solution**: Verify firewall settings
- **Solution**: Restart the ESP8266

## 📊 Debug Output

### Serial Monitor Output

```
ESP8266 Afterburner Starting...
=== Hardware Test ===
Testing PWM pin 14 (D5)...
Testing LED pin 12 (D6)...
Testing LED strip with simple pattern...
Showing red pattern...
Showing green pattern...
Showing blue pattern...
Clearing LEDs...
Hardware test complete
OLED Display initialization skipped
Forced test colors: Red -> Green
Demo mode disabled - testing real PWM signal
Starting automatic throttle calibration...
=== THROTTLE CALIBRATION STARTED ===
Move your throttle stick from MIN to MAX several times
Calibration will complete automatically after 100 samples
Calibration: 10 samples, Min: 988 us, Max: 2011 us
...
=== THROTTLE CALIBRATION COMPLETE ===
Min Pulse: 988 us
Max Pulse: 2011 us
Range: 1023 us
Calibration saved!
ESP8266 Afterburner Ready!
Throttle: 0.07, Mode: 1, LEDs: 19, Calibrated: 988-2011 us
PWM: 988 us -> 0.07 throttle
LED Colors - Start: [255,0,0], End: [0,255,0], Throttle: 0.07
First LED Color: R=30, G=0, B=0
```

## 🔄 Recent Changes (v2.0)

### Added Features

- ✅ **Automatic Throttle Calibration**: Works with any transmitter
- ✅ **Enhanced PWM Detection**: Improved signal validation
- ✅ **Real-time Debug Output**: Comprehensive serial monitoring
- ✅ **Dynamic Color Mapping**: Uses calibrated PWM range
- ✅ **Improved Error Handling**: Better failsafe mechanisms

### Removed Features

- ❌ **OLED Display Support**: Removed to reduce complexity
- ❌ **Manual PWM Configuration**: Replaced with auto-calibration
- ❌ **Excessive Debug Output**: Streamlined for clarity

### Technical Improvements

- 🔧 **Faster Throttle Response**: Increased smoothing factor
- 🔧 **Better Signal Validation**: Improved PWM range detection
- 🔧 **Enhanced LED Effects**: More responsive color transitions
- 🔧 **Optimized Memory Usage**: Reduced RAM footprint

## 📝 Version History

### v2.0 (Current) - NodeMCU V3 Edition

- Automatic throttle calibration
- Enhanced debugging and monitoring
- Improved PWM signal handling
- Removed OLED display dependencies
- Optimized for NodeMCU V3

### v1.0 (Previous)

- Basic PWM throttle input
- Manual configuration required
- OLED display support
- Basic LED effects

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

---

**NodeMCU V3 Edition** - Optimized for ESP8266 NodeMCU V3 with automatic calibration and enhanced features.
