# AfterburnerControl

A React Native application for controlling ESP32-based WS2812 LED afterburner effects for RC jets via Bluetooth Low Energy (BLE).

## Features

- **BLE Connection**: Connect to ESP32 devices named "ABurner"
- **Real-time Control**: Adjust LED effects in real-time
- **Color Management**: Set start and end colors with intuitive color picker (RGB sliders and predefined colors)
- **Mode Selection**: Choose from Linear, Ease, and Pulse animation modes
- **Parameter Control**: Adjust speed, brightness, number of LEDs, and afterburner threshold
- **Status Monitoring**: Real-time display of throttle position and current mode
- **Preset Management**: Save settings to device memory

## Hardware Requirements

- ESP32 Dev Board (e.g., ESP32-DevKitC or WROOM32)
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

1. **Connect to Device**: Tap the "Connect" button to scan for and connect to your ESP32 device named "ABurner"

2. **Adjust Settings**:

   - **Mode**: Select Linear, Ease, or Pulse animation mode
   - **Colors**: Set start and end colors using an intuitive color picker
   - **Speed**: Adjust animation speed (100-5000ms range)
   - **Brightness**: Set LED brightness (10-255 range)
   - **Number of LEDs**: Configure LED count (1-100 range, default: 45)
   - **Afterburner Threshold**: Set throttle percentage for afterburner effect (0-100% range)

3. **Send Settings**: Use individual "Send" buttons or "Push All Settings" to apply changes

4. **Save Preset**: Save current settings to device memory for persistence

5. **Monitor Status**: View real-time throttle position and current mode

## BLE Protocol

The app communicates with the ESP32 using a custom BLE service with the following characteristics:

- **Service UUID**: `b5f9a000-2b6c-4f6a-93b1-2f1f5f9ab000`
- **Mode**: `b5f9a001-2b6c-4f6a-93b1-2f1f5f9ab001` (u8)
- **Start Color**: `b5f9a002-2b6c-4f6a-93b1-2f1f5f9ab002` (RGB)
- **End Color**: `b5f9a003-2b6c-4f6a-93b1-2f1f5f9ab003` (RGB)
- **Speed**: `b5f9a004-2b6c-4f6a-93b1-2f1f5f9ab004` (u16le)
- **Brightness**: `b5f9a005-2b6c-4f6a-93b1-2f1f5f9ab005` (u8)
- **Num LEDs**: `b5f9a006-2b6c-4f6a-93b1-2f1f5f9ab006` (u16le)
- **AB Threshold**: `b5f9a007-2b6c-4f6a-93b1-2f1f5f9ab007` (u8)
- **Save Preset**: `b5f9a008-2b6c-4f6a-93b1-2f1f5f9ab008` (u8)
- **Status**: `b5f9a009-2b6c-4f6a-93b1-2f1f5f9ab009` (JSON notify)

## Permissions

### Android

- `BLUETOOTH_SCAN`
- `BLUETOOTH_CONNECT`
- `ACCESS_FINE_LOCATION`
- `ACCESS_COARSE_LOCATION`

### iOS

- `NSBluetoothAlwaysUsageDescription`
- `NSBluetoothPeripheralUsageDescription`
- `NSLocationWhenInUseUsageDescription`

## Dependencies

The app uses the following key dependencies:

- **react-native-ble-plx**: Bluetooth Low Energy communication
- **react-native-color-picker**: Intuitive color selection interface
- **@react-native-community/slider**: Required by the color picker component

## Project Structure

```
src/
├── ble/
│   ├── bleManager.ts      # BLE manager singleton
│   ├── device.ts          # Device connection helpers
│   └── uuids.ts           # BLE UUIDs and constants
├── components/
│   └── ColorInput.tsx     # Color picker component
└── screens/
    └── AfterburnerScreen.tsx  # Main control screen
```

## Troubleshooting

### Connection Issues

- Ensure Bluetooth is enabled on your device
- Verify the ESP32 device is advertising with name "ABurner"
- Check that the ESP32 firmware is running and BLE service is active
- Grant location permissions (required for BLE scanning on Android)

### Permission Issues

- On Android 12+, ensure "Nearby Devices" permission is granted
- On iOS, ensure Bluetooth permissions are granted in Settings

### Build Issues

- Clean and rebuild: `npx react-native clean`
- Reset Metro cache: `npx react-native start --reset-cache`

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Based on the ESP32 WS2812 Afterburner specification
- Uses react-native-ble-plx for BLE communication
- Built with React Native 0.81.1
