# ESP8266 NodeMCU V3 Migration Guide

This document outlines the complete migration of the Afterburner project from ESP32 to ESP8266 NodeMCU V3.

## Overview

The project has been successfully migrated from ESP32 to ESP8266 NodeMCU V3. The main changes include:

- Replaced BLE with WiFi/WebSocket communication
- Updated GPIO pin assignments for ESP8266
- Replaced ESP32-specific libraries with ESP8266-compatible alternatives
- Adapted memory management for ESP8266 constraints

## Key Changes Made

### 1. PlatformIO Configuration (`platformio.ini`)

**Updated platform and board:**

```ini
# Before (ESP32)
[env:esp32dev]
platform = espressif32
board = esp32dev

# After (ESP8266)
[env:nodemcuv2]
platform = espressif8266
board = nodemcuv2
```

**Updated dependencies:**

```ini
# Removed ESP32-specific BLE library
- h2zero/NimBLE-Arduino@^2.3.4

# Added ESP8266 WiFi and WebSocket libraries
+ links2004/WebSockets@^2.4.1
+ ESP8266WiFi
+ ESP8266WebServer
```

### 2. Communication Protocol

**Before (ESP32):** BLE (Bluetooth Low Energy)
**After (ESP8266):** WiFi Access Point + WebSocket

- **WiFi AP:** Creates "Afterburner_AP" network
- **Web Server:** HTTP server on port 80 with web interface
- **WebSocket:** Real-time communication on port 81
- **Password:** "afterburner123"

### 3. GPIO Pin Assignments

**Updated pin assignments for ESP8266 NodeMCU V3:**

| Function          | ESP32 Pin | ESP8266 Pin | NodeMCU Label |
| ----------------- | --------- | ----------- | ------------- |
| Throttle Input    | GPIO5     | GPIO14      | D5            |
| LED Strip         | GPIO4     | GPIO12      | D6            |
| I2C SDA           | GPIO4     | GPIO4       | D2            |
| I2C SCL           | GPIO5     | GPIO5       | D1            |
| Navigation Button | GPIO2     | GPIO0       | FLASH         |

### 4. Storage System

**Before (ESP32):** Preferences library
**After (ESP8266):** EEPROM library

- Replaced ESP32's Preferences with ESP8266's EEPROM
- Implemented custom serialization for settings storage
- Added magic number validation for data integrity
- Settings are stored in EEPROM with 512-byte allocation

### 5. Library Compatibility

**Replaced/Updated Libraries:**

- `Preferences.h` → `EEPROM.h`
- `NimBLEDevice.h` → `ESP8266WiFi.h` + `WebSocketsServer.h`
- Updated ArduinoJson usage for ESP8266 compatibility

### 6. Memory Management

**ESP8266 Constraints:**

- **RAM:** 80KB (vs ESP32's 520KB)
- **Flash:** 4MB (vs ESP32's 4MB)
- **Current Usage:**
  - RAM: 49.3% (40,372 bytes from 81,920 bytes)
  - Flash: 34.5% (360,499 bytes from 1,044,464 bytes)

## New Features

### 1. Web Interface

The ESP8266 version includes a complete web interface accessible at `http://192.168.4.1`:

- **Real-time control:** Adjust all settings via web form
- **Live status:** View throttle and mode in real-time
- **Responsive design:** Works on mobile and desktop
- **WebSocket communication:** Instant updates without page refresh

### 2. WiFi Access Point Mode

- **SSID:** "Afterburner_AP"
- **Password:** "afterburner123"
- **IP Address:** 192.168.4.1
- **Web Server:** Port 80
- **WebSocket:** Port 81

### 3. Enhanced Connectivity

- **Multiple clients:** Supports multiple web clients simultaneously
- **Real-time updates:** WebSocket provides instant status updates
- **Cross-platform:** Works with any device with a web browser

## Build Results

### Successful Build

```
Environment    Status    Duration
-------------  --------  ------------
nodemcuv2      SUCCESS   00:01:05.424
```

### Memory Usage

- **RAM:** 49.3% (40,372 bytes from 81,920 bytes)
- **Flash:** 34.5% (360,499 bytes from 1,044,464 bytes)

## Usage Instructions

### 1. Flashing the Firmware

```bash
pio run -e nodemcuv2 --target upload
```

### 2. Connecting to the Device

1. Power on the ESP8266
2. Look for WiFi network "Afterburner_AP"
3. Connect with password "afterburner123"
4. Open web browser and navigate to `http://192.168.4.1`

### 3. Web Interface Features

- **Mode Selection:** Static, Pulse, Afterburner
- **Color Control:** RGB sliders for start and end colors
- **Speed Control:** Animation speed adjustment
- **Brightness Control:** LED brightness slider
- **LED Count:** Number of LEDs configuration
- **Threshold Control:** Afterburner activation threshold

## Hardware Connections

### ESP8266 NodeMCU V3 Pinout

```
NodeMCU V3 Pin Mapping:
D0  (GPIO16) - Boot mode
D1  (GPIO5)  - I2C SCL
D2  (GPIO4)  - I2C SDA
D3  (GPIO0)  - FLASH button (Navigation)
D4  (GPIO2)  - Built-in LED
D5  (GPIO14) - Throttle Input
D6  (GPIO12) - LED Strip Data
D7  (GPIO13) - SPI MOSI
D8  (GPIO15) - SPI SS
```

### Wiring Diagram

```
ESP8266 NodeMCU V3:
├── D5 (GPIO14) → Throttle PWM Input
├── D6 (GPIO12) → LED Strip Data
├── D2 (GPIO4)  → OLED Display SDA
├── D1 (GPIO5)  → OLED Display SCL
└── D3 (GPIO0)  → Navigation Button (FLASH)
```

## Limitations and Considerations

### 1. Performance Differences

- **Processing Power:** ESP8266 is slower than ESP32
- **Memory:** Limited RAM requires careful memory management
- **WiFi vs BLE:** WiFi uses more power but provides better range

### 2. Hardware Limitations

- **Single Core:** ESP8266 has only one core vs ESP32's dual-core
- **GPIO Count:** Fewer GPIO pins available
- **ADC:** Only one ADC channel vs ESP32's multiple channels

### 3. Library Compatibility

- Some ESP32-specific libraries are not available for ESP8266
- WebSocket library provides similar functionality to BLE
- EEPROM library replaces Preferences for persistent storage

## Migration Benefits

### 1. Cost Effectiveness

- ESP8266 NodeMCU V3 is significantly cheaper than ESP32
- Reduced component costs for mass production

### 2. Power Efficiency

- ESP8266 can be more power-efficient for simple applications
- WiFi can be disabled when not needed

### 3. Accessibility

- Web interface is more accessible than mobile apps
- Cross-platform compatibility
- No app installation required

### 4. Development Simplicity

- Web development is more familiar to many developers
- Easier to debug and modify interface
- Real-time updates without app updates

## Future Enhancements

### 1. OTA Updates

- Implement Over-The-Air firmware updates
- Web-based firmware upload interface

### 2. Advanced Web Features

- Save/load preset configurations
- Advanced animation controls
- Real-time spectrum analyzer

### 3. Network Features

- WiFi client mode for internet connectivity
- Remote control via internet
- Cloud-based configuration sync

## Troubleshooting

### Common Issues

1. **WiFi Connection Problems**

   - Ensure device is powered correctly
   - Check for conflicting WiFi networks
   - Verify password is correct

2. **Web Interface Not Loading**

   - Clear browser cache
   - Try different browser
   - Check IP address is correct

3. **Settings Not Saving**
   - Check EEPROM initialization
   - Verify magic number in EEPROM
   - Monitor serial output for errors

### Debug Information

- Serial monitor at 115200 baud
- WebSocket connection status in web interface
- EEPROM read/write confirmation messages

## Conclusion

The migration to ESP8266 NodeMCU V3 has been successful, providing a cost-effective alternative to the ESP32 version while maintaining all core functionality. The web interface offers improved accessibility and ease of use compared to the BLE mobile app approach.

The project now supports both ESP32 and ESP8266 platforms, allowing users to choose based on their specific requirements and budget constraints.
