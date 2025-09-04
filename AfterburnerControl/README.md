# AfterburnerControl

A React Native application for controlling ESP32-based WS2812 LED afterburner effects for RC jets via Bluetooth Low Energy (BLE), featuring enhanced calibration, theme persistence, and real-time monitoring.

## Features

- **BLE Connection**: Connect to ESP32 devices named "ABurner"
- **Real-time Control**: Adjust LED effects in real-time
- **Enhanced Throttle Calibration**: Multi-position validation with progress monitoring
- **Color Management**: Set start and end colors with intuitive color picker (RGB sliders and predefined colors)
- **Mode Selection**: Choose from Linear, Ease, and Pulse animation modes
- **Parameter Control**: Adjust speed, brightness, number of LEDs, and afterburner threshold
- **Status Monitoring**: Real-time display of throttle position and current mode
- **Preset Management**: Save settings to device memory
- **Theme Persistence**: Dark/light theme saved to device storage
- **Calibration Progress**: Visual feedback showing min/max visit counts

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

### 1. **Connect to Device**:

- Tap the "Connect" button to scan for and connect to your ESP32 device named "ABurner"
- Grant necessary Bluetooth and location permissions

### 2. **Throttle Calibration**:

- **Start Calibration**: Tap "Start Calibration" to begin the process
- **Multiple Positions**: Move throttle to min/max positions multiple times
- **Progress Monitoring**: App shows visit counts (e.g., "Min visits: 2/3, Max visits: 1/3")
- **Automatic Completion**: App automatically detects when calibration is complete
- **Reset Calibration**: Use "Reset Calibration" if needed

### 3. **Adjust Settings**:

- **Mode**: Select Linear, Ease, or Pulse animation mode
- **Colors**: Set start and end colors using an intuitive color picker
- **Speed**: Adjust animation speed (100-5000ms range) for all effects
- **Brightness**: Set LED brightness (10-255 range)
- **Number of LEDs**: Configure LED count (1-100 range, default: 45)
- **Afterburner Threshold**: Set throttle percentage for afterburner effect (0-100% range)

### 4. **Send Settings**:

- Use individual "Send" buttons or "Push All Settings" to apply changes

### 5. **Save Preset**:

- Save current settings to device memory for persistence

### 6. **Monitor Status**:

- View real-time throttle position and current mode
- Check calibration status and progress

### 7. **Theme Control**:

- **Toggle Theme**: Tap the theme button (☀️/🌙) to switch between light and dark themes
- **Reset Theme**: Long-press the theme button to reset to default light theme
- **Theme Persistence**: Theme preference is automatically saved to device storage

## Enhanced Calibration System

The app now features an advanced calibration system that ensures accurate throttle readings:

### Calibration Process

1. **Start**: App sends calibration start command to ESP32
2. **Progress Monitoring**: Real-time updates showing visit counts to min/max positions
3. **Stability Validation**: ESP32 ensures readings are consistent before accepting
4. **Completion Detection**: App automatically detects when calibration is complete
5. **Status Updates**: Real-time feedback during the entire process

### Calibration Features

- **Multi-position validation**: Requires multiple visits to min/max positions
- **Stability checking**: Ensures readings are consistent before saving
- **Time-based validation**: Prevents immediate saving for better accuracy
- **Progress visualization**: Clear feedback showing current progress
- **Automatic completion**: No manual intervention required

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
- **Throttle Calibration**: `b5f9a00a-2b6c-4f6a-93b1-2f1f5f9ab00a` (u8)
- **Throttle Calibration Reset**: `b5f9a00b-2b6c-4f6a-93b1-2f1f5f9ab00b` (u8)
- **Throttle Calibration Status**: `b5f9a00c-2b6c-4f6a-93b1-2f1f5f9ab00c` (notify)

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

- **react-native-ble-manager**: Bluetooth Low Energy communication
- **react-native-color-picker**: Intuitive color selection interface
- **@react-native-community/slider**: Required by the color picker component
- **@react-native-async-storage/async-storage**: Theme preference persistence

## Project Structure

```
src/
├── ble/
│   ├── bleManager.ts      # BLE manager singleton with enhanced monitoring
│   ├── device.ts          # Device connection helpers and calibration
│   └── uuids.ts           # BLE UUIDs and constants
├── components/
│   └── ColorInput.tsx     # Color picker component with theme support
└── screens/
    └── AfterburnerScreen.tsx  # Main control screen with theme management
```

## Theme System

The app features a comprehensive theme system:

### Light Theme (Default)

- Clean, bright interface
- High contrast for outdoor use
- Standard color scheme

### Dark Theme

- Reduced eye strain in low-light conditions
- Enhanced visibility for device status information
- Consistent styling across all components

### Theme Features

- **Automatic Persistence**: Theme choice saved to device storage
- **Immediate Application**: Theme changes apply instantly
- **Reset Capability**: Long-press to return to default theme
- **Component Support**: All UI elements support both themes

## Troubleshooting

### Connection Issues

- Ensure Bluetooth is enabled on your device
- Verify the ESP32 device is advertising with name "ABurner"
- Check that the ESP32 firmware is running and BLE service is active
- Grant location permissions (required for BLE scanning on Android)

### Calibration Issues

- **Move throttle slowly**: Ensure stable readings at min/max positions
- **Multiple visits**: Visit each position multiple times for validation
- **Check progress**: Monitor the calibration progress in the app
- **Reset if needed**: Use reset calibration if process gets stuck

### Permission Issues

- On Android 12+, ensure "Nearby Devices" permission is granted
- On iOS, ensure Bluetooth permissions are granted in Settings

### Build Issues

- Clean and rebuild: `npx react-native clean`
- Reset Metro cache: `npx react-native start --reset-cache`

### Debug Information

- **App Logs**: Only critical error information is logged
- **BLE Status**: Real-time connection and calibration status
- **Calibration Progress**: Visual feedback during calibration process

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

### Code Style Guidelines

- Follow existing code formatting
- Add comments for complex logic
- Update documentation for new features
- Include tests when possible
- Keep debug logs minimal and essential

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Based on the ESP32 WS2812 Afterburner specification
- Uses react-native-ble-manager for BLE communication
- Built with React Native 0.81.1
- Enhanced calibration system for improved accuracy
- Theme system for better user experience
