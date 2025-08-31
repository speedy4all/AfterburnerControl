# AfterburnerControl

A React Native application for controlling ESP8266-based WS2812 LED afterburner effects for RC jets via WebSocket over WiFi.

## Features

- **WebSocket Connection**: Connect to ESP8266 devices via WiFi
- **Real-time Control**: Adjust LED effects in real-time
- **Color Management**: Set start and end colors with intuitive color picker (RGB sliders and predefined colors)
- **Mode Selection**: Choose from Static, Pulse, and Afterburner animation modes
- **Parameter Control**: Adjust speed, brightness, number of LEDs, and afterburner threshold
- **Status Monitoring**: Real-time display of throttle position and current mode
- **Throttle Calibration**: Start and reset throttle calibration
- **Reliable Communication**: Optimized to prevent controller crashes
- **Simplified Interface**: Clean, intuitive design focused on reliability

## Hardware Requirements

- ESP8266 Dev Board (e.g., NodeMCU, Wemos D1 Mini, or ESP-12F)
- WS2812B LED ring (45 LEDs recommended)
- 5V UBEC/DC-DC converter (≥5A rated)
- 74HCT245 level shifter (3.3V→5V)
- 330Ω series resistor
- 1000µF electrolytic capacitor
- PWM throttle input (1000-2000µs)

## Installation

### Prerequisites

- Node.js (v20.19.4 or higher)
- React Native CLI
- Android Studio (for Android development)
- Xcode (for iOS development, macOS only)

### Setup

1. Clone the repository:

```bash
git clone <repository-url>
cd AfterburnerControl
```

2. Install dependencies:

```bash
npm install
```

3. For iOS (macOS only):

```bash
cd ios && pod install && cd ..
```

## Running the App

### Android

1. Connect an Android device or start an emulator
2. Run the app:

```bash
npx react-native run-android
```

### iOS (macOS only)

1. Open the project in Xcode:

```bash
open ios/AfterburnerControl.xcworkspace
```

2. Run the app:

```bash
npx react-native run-ios
```

## Usage

### 🎯 Simplified Interface

The app now uses a **"Push All Settings"** approach for maximum reliability:

1. **Connect to Device**:

   - Connect your mobile device to the ESP8266's WiFi network (`Afterburner_AP`)
   - Tap the "Connect" button to establish WebSocket connection

2. **Adjust Settings**:

   - **Mode**: Select Static, Pulse, or Afterburner animation mode
   - **Colors**: Set start and end colors using an intuitive color picker
   - **Speed**: Adjust animation speed (100-5000ms range)
   - **Brightness**: Set LED brightness (10-255 range)
   - **Number of LEDs**: Configure LED count (1-100 range, default: 45)
   - **Afterburner Threshold**: Set throttle percentage for afterburner effect (0-100% range)

3. **🚀 Push All Settings**: Tap the prominent green button to send all changes at once

   - This prevents controller crashes that can occur with individual setting updates
   - All settings are sent together in a single, reliable operation

4. **Throttle Calibration**:

   - **Start Calibration**: Begin automatic throttle calibration
   - **Reset to Default Values**: Use default PWM range (900-2100 μs)

5. **Monitor Status**: View real-time throttle position, mode, and calibration status

### 💡 Key Benefits

- **No More Crashes**: Eliminates controller restarts when changing settings
- **Intuitive Design**: Clear visual feedback and easy-to-use controls
- **Reliable Communication**: Optimized WebSocket protocol for stability
- **Visual Color Picker**: Easy color selection with predefined palettes

## WebSocket Protocol

The app communicates with the ESP8266 using WebSocket over WiFi with the following configuration:

- **WiFi Network**: `Afterburner_AP` (password: `afterburner123`)
- **Device IP**: `192.168.4.1`
- **WebSocket Port**: `81`
- **Connection URL**: `ws://192.168.4.1:81/`

### Message Format

Settings are sent as complete JSON objects (no partial updates):

```json
{
  "mode": 0,
  "startColor": [255, 0, 0],
  "endColor": [0, 0, 255],
  "speedMs": 1200,
  "brightness": 200,
  "numLeds": 45,
  "abThreshold": 80
}
```

Status updates are received as JSON:

```json
{
  "type": "status",
  "thr": 0.75,
  "mode": 2,
  "signalValid": true,
  "pulseCount": 150,
  "calibrating": false,
  "calibrationComplete": true,
  "minPulse": 988,
  "maxPulse": 2011,
  "pulseRange": 1023
}
```

Commands are sent as JSON:

```json
{
  "type": "command",
  "command": "start_calibration"
}
```

## Permissions

### Android

- `ACCESS_NETWORK_STATE`
- `ACCESS_WIFI_STATE`
- `INTERNET`

### iOS

- No special permissions required for WebSocket connections

## Dependencies

The app uses the following key dependencies:

- **WebSocket API**: Built-in WebSocket support for WiFi communication
- **@react-native-community/slider**: Required by the color picker component

## Project Structure

```
src/
├── websocket/
│   ├── websocketManager.ts  # WebSocket manager singleton
│   ├── device.ts           # Device connection helpers
│   └── constants.ts        # WebSocket constants and modes
├── components/
│   └── ColorInput.tsx      # Color picker component
└── screens/
    └── AfterburnerScreen.tsx  # Main control screen
```

## Troubleshooting

### Connection Issues

- Ensure the ESP8266 device is powered on and running the firmware
- Connect your mobile device to the `Afterburner_AP` WiFi network
- Verify the device IP address is `192.168.4.1`
- Check that the WebSocket server is running on port 81

### Network Issues

- Ensure WiFi is enabled on your mobile device
- Verify you're connected to the correct WiFi network
- Check that the device is within WiFi range

### Controller Crashes

- **Always use "Push All Settings"** instead of individual controls
- Ensure you're connected to the Afterburner_AP WiFi network
- Check that the ESP8266 is powered properly
- If crashes persist, try resetting the ESP8266 device

### Build Issues

- Clean and rebuild: `npx react-native clean`
- Reset Metro cache: `npx react-native start --reset-cache`

## Recent Changes (v2.1.0)

### ✅ Improvements

- **Simplified Interface**: Removed individual setting controls for reliability
- **"Push All Settings" Approach**: Send complete settings at once to prevent crashes
- **Enhanced Color Picker**: Visual RGB color selection with predefined palettes
- **Improved Error Handling**: Better validation and crash prevention
- **Streamlined UI**: Cleaner, more intuitive interface
- **Throttle Calibration UI**: Visual feedback for calibration status

### ❌ Removed Features

- **Individual Setting Controls**: Removed to prevent controller crashes
- **Test Ping/Connection**: Removed debugging features for cleaner interface
- **Individual Send Buttons**: Replaced with single "Push All Settings" button

### 🔧 Technical Improvements

- **Atomic Settings Updates**: Complete settings replacement instead of partial updates
- **Enhanced Validation**: Comprehensive input validation
- **Optimized WebSocket Communication**: Reduced message frequency and improved reliability
- **Better Error Handling**: Robust connection and communication error handling

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Based on the ESP8266 WS2812 Afterburner specification
- Uses WebSocket for WiFi communication
- Built with React Native 0.81.1
