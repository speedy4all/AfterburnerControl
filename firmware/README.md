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
- **🛡️ Crash Prevention**: Optimized to prevent controller restarts during settings updates

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

**🎯 Simplified Interface**: The app now uses a "Push All Settings" approach for maximum reliability:

- **Mode Selection**: Choose between Static, Pulse, and Afterburner
- **Color Control**: Set start and end colors (RGB) with visual color picker
- **Brightness**: Adjust LED brightness (10-255)
- **Speed**: Control animation speed (100-5000ms)
- **LED Count**: Set number of LEDs (1-300)
- **Afterburner Threshold**: Set afterburner activation point (0-100%)
- **🚀 Push All Settings**: Send all changes at once to prevent controller crashes

**💡 How to Use**:

1. Adjust any settings using the intuitive controls
2. Tap "🚀 Push All Settings to Device" to send all changes
3. The device will update immediately without crashes

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

### Reliability Settings

```cpp
#define SETTINGS_UPDATE_RATE_LIMIT 200  // Minimum time between settings updates (ms)
#define JSON_BUFFER_SIZE 512            // JSON parsing buffer size
#define EEPROM_SAVE_DELAY 5             // Delay before/after EEPROM saves (ms)
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

### Controller Crashes

**Problem**: Controller restarts when changing settings

- **Solution**: Use "Push All Settings" instead of individual controls
- **Solution**: Ensure you're connected to the Afterburner_AP WiFi network
- **Solution**: Check that the ESP8266 is powered properly
- **Solution**: If crashes persist, try resetting the ESP8266 device

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
WiFi AP started. SSID: Afterburner_AP IP Address: 192.168.4.1
ESP8266 Afterburner Ready!
Throttle: 0.07, Mode: 1, LEDs: 19, Calibrated: 988-2011 us
PWM: 988 us -> 0.07 throttle
LED Colors - Start: [255,0,0], End: [0,255,0], Throttle: 0.07
First LED Color: R=30, G=0, B=0
```

### Settings Update Debug

```
WebSocket text received: {"startColor":[255,165,0],"endColor":[255,0,255],"mode":2,"speedMs":1500,"brightness":200,"numLeds":45,"abThreshold":80}
JSON length: 89
JSON parsed successfully
JSON document size: 256 bytes
=== Processing Settings ===
Current settings - Mode: 1, Speed: 1200, Brightness: 200, LEDs: 45, Threshold: 80
Current colors - Start: [255,0,0], End: [0,0,255]
=== SETTINGS CHANGED ===
Final settings - Mode: 2, Speed: 1500, Brightness: 200, LEDs: 45, Threshold: 80
Final colors - Start: [255,165,0], End: [255,0,255]
Settings to save - Mode: 2, Speed: 1500, Brightness: 200, LEDs: 45, Threshold: 80
Saving settings to EEPROM in main loop...
Settings saved successfully
```

## 🔄 Recent Changes (v2.1.0)

### ✅ Added Features

- **🛡️ Crash Prevention**: Multiple layers of validation and error handling
- **Atomic Settings Updates**: Complete settings replacement instead of partial updates
- **Enhanced Validation**: Comprehensive input validation for all settings
- **Deferred EEPROM Saves**: Moved saves to main loop to prevent watchdog resets
- **Rate Limiting**: Prevents overwhelming the controller with rapid updates
- **Memory Optimization**: Reduced JSON buffer sizes and improved memory management

### ❌ Removed Features

- **OLED Display Support**: Removed to reduce complexity
- **Manual PWM Configuration**: Replaced with auto-calibration
- **Individual Setting Updates**: Now only accepts complete settings objects
- **Excessive Debug Output**: Streamlined for clarity

### 🔧 Technical Improvements

- **Watchdog Timer Management**: Proper feeding during critical operations
- **Optimized WebSocket Communication**: Reduced message frequency and improved reliability
- **Better Error Handling**: Robust connection and communication error handling
- **Enhanced JSON Parsing**: Improved type conversion and validation
- **Streamlined EEPROM Operations**: More efficient settings storage

### 🛡️ Reliability Improvements

- **Crash Prevention**: Multiple layers of validation and error handling
- **Watchdog Timer Management**: Proper feeding during critical operations
- **Rate Limiting**: Prevents overwhelming the controller with rapid updates
- **Memory Optimization**: Reduced memory fragmentation and improved stability
- **Atomic Updates**: Complete settings replacement ensures consistency

## 📝 Version History

### v2.1.0 (Current) - Reliability Edition

- **Crash Prevention**: Optimized to prevent controller restarts
- **Simplified Communication**: Only accepts complete settings updates
- **Enhanced Validation**: Comprehensive input validation
- **Memory Optimization**: Improved memory management
- **Deferred EEPROM Saves**: Moved saves to main loop

### v2.0 (Previous) - NodeMCU V3 Edition

- Automatic throttle calibration
- Enhanced debugging and monitoring
- Improved PWM signal handling
- Removed OLED display dependencies
- Optimized for NodeMCU V3

### v1.0 (Original)

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

**NodeMCU V3 Edition v2.1.0** - The most reliable ESP8266 afterburner controller with crash prevention and optimized communication! 🛩️
