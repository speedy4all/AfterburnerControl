# Changelog

All notable changes to the ESP32 C3 Afterburner Project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased] - 2024-12-19

### Added

- **Enhanced Throttle Calibration System**

  - Multi-position validation requiring multiple visits to min/max positions
  - Stability checking to ensure readings are consistent before saving
  - Time-based validation preventing immediate saving for better accuracy
  - Real-time progress monitoring with visit counts
  - Automatic completion detection

- **Speed Setting Implementation**

  - Animation speed control (100-5000ms) for all LED effects
  - Speed-controlled pulse mode afterburner effects
  - Speed-controlled breathing effects for Ease and Pulse modes
  - Speed-controlled flicker animation cycles
  - Speed-controlled sparkle frequency during afterburner

- **Advanced Mobile App Features**

  - Theme persistence with dark/light theme support
  - Real-time calibration progress monitoring
  - Enhanced UI visibility in both light and dark themes
  - Calibration progress visualization showing min/max visit counts
  - Automatic status updates and completion detection

- **Enhanced BLE Communication**
  - Real-time calibration status notifications
  - Progress data transmission (minVisits, maxVisits)
  - Enhanced status monitoring and connection health checks
  - Improved error handling and recovery

### Changed

- **Firmware Debug Logging**

  - Removed excessive debug prints from all components
  - Kept only essential error and status messages
  - Improved code readability and focus on critical information
  - Cleaned up verbose BLE event logging

- **Mobile App Debug Logging**

  - Removed excessive console.log statements
  - Kept only critical error logs and essential status updates
  - Improved app performance and reduced log noise
  - Cleaned up verbose BLE communication logging

- **Settings Management**

  - Enhanced flash memory save/load operations
  - Improved error reporting and verification
  - Better calibration value persistence
  - Enhanced settings validation

- **Throttle Processing**
  - Improved PWM to throttle mapping
  - Enhanced calibration value loading on startup
  - Better handling of invalid calibration states
  - Improved throttle reading stability

### Fixed

- **Compilation Issues**

  - Fixed missing `MIN_PWM_VALUE` constant declaration
  - Resolved `ThrottleReader` forward declaration issues
  - Fixed function signature mismatches in LED effects
  - Corrected type definition issues

- **BLE Communication Issues**

  - Fixed byte order for throttle calibration values (little-endian)
  - Resolved characteristic notification subscription issues
  - Fixed calibration status callback type definitions
  - Corrected BLE event listener setup

- **Mobile App Issues**

  - Fixed theme toggle functionality
  - Resolved calibration status display issues
  - Fixed dark theme visibility problems
  - Corrected color component theme support

- **Calibration System Issues**
  - Fixed immediate saving of calibration values
  - Resolved app not recognizing calibration completion
  - Fixed throttle value showing as null after calibration
  - Corrected calibration status synchronization

### Technical Improvements

- **Code Structure**

  - Added `constants.h` for system-wide constants
  - Enhanced error handling throughout the codebase
  - Improved function parameter passing
  - Better separation of concerns

- **Performance Optimizations**

  - Reduced unnecessary debug output
  - Optimized BLE communication
  - Improved LED effect rendering
  - Enhanced memory management

- **User Experience**
  - More intuitive calibration process
  - Better visual feedback during operations
  - Improved theme system with persistence
  - Enhanced error reporting and recovery

## [Previous Versions]

### [v1.0.0] - Initial Release

- Basic ESP32 C3 afterburner functionality
- WS2812B LED control
- Basic BLE communication
- OLED display interface
- Simple throttle input processing

---

## Migration Notes

### For Users

- **Calibration Process**: The new calibration system requires multiple visits to min/max positions. Follow the on-screen progress indicators.
- **Speed Settings**: The speed setting now controls animation timing. Lower values (100ms) create faster effects, higher values (5000ms) create slower effects.
- **Theme System**: The app now remembers your theme preference. Use the theme toggle button to switch between light and dark themes.

### For Developers

- **Debug Logging**: Debug output has been significantly reduced. Check the troubleshooting section for essential information.
- **BLE Protocol**: New characteristics have been added for calibration status. Update any custom BLE clients accordingly.
- **Function Signatures**: Some LED effect functions now require the settings object as a parameter.

---

## Known Issues

- None currently identified

## Planned Features

- WiFi connectivity for web interface
- Multiple LED strip support
- Sound effects simulation
- Temperature monitoring
- Preset management system
- Advanced engine simulation effects

---

## Contributing

When contributing to this project, please:

1. Follow the existing code style
2. Add appropriate documentation
3. Include tests when possible
4. Keep debug logs minimal and essential
5. Update this changelog for significant changes

---

## Support

For support and questions:

1. Check the troubleshooting sections in the documentation
2. Review the wiring diagrams
3. Monitor Serial output for debug information
4. Check mobile app for calibration status
5. Open an issue with detailed problem description
