# Version History - ESP8266 Afterburner Controller

## v2.0.0 - NodeMCU V3 Edition (Current)

**Release Date**: December 2024  
**Target Platform**: ESP8266 NodeMCU V3  
**Status**: Stable Release

### 🎯 Major Features

#### Automatic Throttle Calibration

- **Auto-Detection**: Automatically detects any transmitter's PWM range
- **Universal Compatibility**: Works with Futaba, Spektrum, FlySky, and other transmitters
- **No Manual Configuration**: Eliminates the need for manual PWM range setup
- **Real-time Calibration**: 100-sample calibration process (~5-10 seconds)

#### Enhanced Hardware Support

- **NodeMCU V3 Optimized**: Specifically tested and optimized for NodeMCU V3
- **Improved Pin Mapping**: Clear D5/D6 pin assignments for throttle and LED
- **Power Management**: Better handling of 5V LED strip power requirements
- **Signal Stability**: Enhanced PWM signal processing and validation

#### Advanced Debugging

- **Comprehensive Monitoring**: Real-time serial output for all system components
- **Calibration Progress**: Live feedback during throttle calibration
- **LED Status**: Color and brightness monitoring
- **Connection Status**: WiFi and WebSocket connection monitoring

### 🔧 Technical Improvements

#### Throttle Processing

- **Faster Response**: Increased smoothing factor from 0.10 to 0.50
- **Better Signal Validation**: Improved PWM range detection (500-2500μs during calibration)
- **Dynamic Mapping**: Uses calibrated min/max values for accurate throttle mapping
- **Failsafe Mechanisms**: Better handling of signal loss and invalid pulses

#### LED Effects

- **Responsive Rendering**: More immediate color transitions
- **Optimized Memory**: Reduced RAM usage for LED arrays
- **Enhanced Effects**: Improved afterburner and sparkle effects
- **Color Accuracy**: Better RGB color interpolation

#### Communication

- **WebSocket Protocol**: Real-time bidirectional communication
- **JSON Handling**: Robust settings serialization/deserialization
- **Error Recovery**: Better connection recovery and error handling
- **Status Updates**: Real-time throttle and mode status broadcasting

### 🗑️ Removed Features

#### OLED Display Support

- **Removed**: All OLED display code and dependencies
- **Reason**: Simplify hardware setup and reduce complexity
- **Alternative**: Mobile app provides all display functionality

#### Manual Configuration

- **Removed**: Manual PWM range configuration
- **Replaced**: Automatic calibration system
- **Benefit**: Works with any transmitter out of the box

#### Excessive Debug Output

- **Streamlined**: Reduced verbose debug messages
- **Focused**: Essential information only
- **Performance**: Faster serial output processing

### 📊 Performance Metrics

#### Calibration Performance

- **Sample Rate**: 100 samples for calibration
- **Duration**: 5-10 seconds typical
- **Accuracy**: ±1μs precision
- **Range**: 500-2500μs detection range

#### LED Performance

- **Max LEDs**: 300 LEDs supported
- **Frame Rate**: 50 FPS (20ms delay)
- **Memory**: ~900 bytes per 100 LEDs
- **Power**: 60mA per LED at full brightness

#### Communication Performance

- **WebSocket**: Real-time bidirectional
- **Update Rate**: 200ms status updates
- **Latency**: <50ms response time
- **Range**: WiFi AP coverage (~30m)

### 🔍 Compatibility

#### Hardware Compatibility

- ✅ **ESP8266 NodeMCU V3** (Primary target)
- ✅ **ESP8266 NodeMCU V2** (Backward compatible)
- ✅ **ESP8266 Wemos D1 Mini** (Pin mapping required)
- ✅ **Other ESP8266 boards** (Manual pin configuration)

#### Transmitter Compatibility

- ✅ **Futaba** (T-FHSS, FASST, S-FHSS)
- ✅ **Spektrum** (DSM2, DSMX)
- ✅ **FlySky** (AFHDS, AFHDS 2A)
- ✅ **FrSky** (D8, D16, ACCESS)
- ✅ **Other PWM transmitters**

#### LED Strip Compatibility

- ✅ **WS2812B** (Primary target)
- ✅ **WS2811** (Compatible)
- ✅ **SK6812** (Compatible)
- ❌ **APA102** (Different protocol)
- ❌ **LPD8806** (Different protocol)

### 🐛 Known Issues

#### v2.0.0 Known Issues

1. **Power Supply**: Large LED strips require external 5V supply
2. **Signal Noise**: Some transmitters may need signal filtering
3. **WiFi Range**: Limited to ~30m in open air
4. **Memory**: Large LED counts may cause stability issues

#### Workarounds

1. **Power**: Use 5V 1A+ power supply for LED strips
2. **Noise**: Add 0.1μF capacitor between signal and ground
3. **Range**: Position device closer to control area
4. **Memory**: Limit LED count to 200 for stability

### 📈 Future Roadmap

#### v2.1.0 Planned Features

- **EEPROM Calibration Storage**: Save calibration across reboots
- **Multiple LED Strips**: Support for multiple afterburner zones
- **Advanced Effects**: More realistic engine simulations
- **Temperature Monitoring**: Thermal protection for LED strips

#### v2.2.0 Planned Features

- **Sound Effects**: Audio simulation via buzzer
- **Preset Management**: Save/load effect configurations
- **Web Interface**: Browser-based control interface
- **OTA Updates**: Over-the-air firmware updates

### 📝 Migration Guide

#### From v1.0 to v2.0

1. **Hardware Changes**:

   - Remove OLED display connections
   - Update pin assignments (D5/D6)
   - Ensure 5V power supply for LED strip

2. **Software Changes**:

   - Update to v2.0 firmware
   - No manual configuration required
   - Automatic calibration on first boot

3. **Configuration**:
   - All settings preserved in EEPROM
   - New calibration data stored automatically
   - Mobile app settings remain compatible

### 🎉 Release Notes

This release represents a major evolution of the ESP8266 Afterburner Controller, focusing on ease of use, reliability, and universal compatibility. The automatic calibration feature eliminates the most common setup issue, while the enhanced debugging makes troubleshooting much easier.

**Key Achievements**:

- ✅ Universal transmitter compatibility
- ✅ Simplified hardware setup
- ✅ Enhanced reliability and stability
- ✅ Comprehensive debugging capabilities
- ✅ Optimized for NodeMCU V3

---

**NodeMCU V3 Edition** - The most user-friendly and reliable version yet!
