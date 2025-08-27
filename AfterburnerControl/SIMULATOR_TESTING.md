# Simulator/Emulator Testing - ESP32 C3 Afterburner App

## ⚠️ Simulator/Emulator Limitations

Both iOS Simulator and Android Emulator have significant limitations when testing Bluetooth Low Energy (BLE) functionality. This document explains what to expect and how to properly test the app.

## 🔍 **Simulator/Emulator Behavior**

### What Works in Simulator/Emulator:

- ✅ App UI and navigation
- ✅ Settings interface
- ✅ Color picker components
- ✅ Input validation
- ✅ App icons and splash screen
- ✅ Basic app functionality

### What Doesn't Work in Simulator/Emulator:

- ❌ Bluetooth Low Energy (BLE) connections
- ❌ Device scanning
- ❌ BLE communication with ESP32
- ❌ Real-time status updates

## 🚨 **Error Messages in Simulator/Emulator**

When running in iOS Simulator or Android Emulator, you may see these errors:

```
Failed to connect to Afterburner: BleError: Device is not authorized to use BluetoothLE
```

**This is expected behavior** - the simulator/emulator will display a warning banner explaining that BLE is not available.

## 📱 **Real Device Testing**

### Requirements for Real Device Testing:

- **iOS Device**: iPhone or iPad with iOS 12.0+
- **Android Device**: Android 6.0+ (API level 23+)
- **Bluetooth**: Device must have Bluetooth 4.0+ (BLE support)
- **Permissions**: App will request Bluetooth permissions on first use

### What Works on Real Devices:

- ✅ Full BLE functionality
- ✅ Device discovery and connection
- ✅ Real-time communication with ESP32
- ✅ Settings transmission and reception
- ✅ Status monitoring
- ✅ All app features

## 🛠️ **Testing Strategy**

### 1. Simulator/Emulator Testing (UI/UX)

```bash
# Test app interface and navigation (iOS)
npx react-native run-ios

# Test app interface and navigation (Android)
npx react-native run-android
```

- Verify all UI components render correctly
- Test navigation and user interactions
- Check input validation and error handling
- Verify app icons and splash screen

### 2. Real Device Testing (BLE Functionality)

```bash
# Build for real device (iOS)
npx react-native run-ios --device

# Build for real device (Android)
npx react-native run-android --device
```

- Test BLE device discovery
- Verify connection to ESP32
- Test all settings transmission
- Monitor real-time status updates

## 🔧 **Development Workflow**

### For UI Development:

1. Use iOS Simulator or Android Emulator for rapid iteration
2. Test interface changes and styling
3. Verify component behavior
4. Ignore BLE-related errors

### For BLE Development:

1. Use real iOS or Android device
2. Test with actual ESP32 hardware
3. Verify BLE communication
4. Debug connection issues

## 📋 **Testing Checklist**

### Simulator/Emulator Testing:

- [ ] App launches without crashes
- [ ] UI components render correctly
- [ ] Navigation works properly
- [ ] Input validation functions
- [ ] Simulator/Emulator warning displays
- [ ] App icons appear correctly

### Real Device Testing:

- [ ] BLE permissions are requested
- [ ] Device scanning works
- [ ] ESP32 device is discovered
- [ ] Connection establishes successfully
- [ ] Settings can be transmitted
- [ ] Status updates are received
- [ ] All features work as expected

## 🎯 **Best Practices**

### Development:

1. **Start with Simulator**: Use simulator for UI development
2. **Test Early on Device**: Don't wait until the end to test BLE
3. **Use Real Hardware**: Always test with actual ESP32 device
4. **Monitor Logs**: Check console output for debugging

### Debugging:

1. **Check Permissions**: Ensure Bluetooth permissions are granted
2. **Verify ESP32**: Confirm ESP32 is powered and advertising
3. **Check Distance**: Ensure devices are within BLE range
4. **Restart if Needed**: Sometimes BLE needs a fresh start

## 🔄 **Common Issues**

### Simulator/Emulator Issues:

- **BLE Errors**: Expected - ignore these
- **Permission Errors**: Expected - simulator/emulator can't handle real permissions
- **Connection Failures**: Expected - no real BLE hardware

### Real Device Issues:

- **Permission Denied**: Check device settings for Bluetooth permissions
- **Device Not Found**: Ensure ESP32 is powered and advertising
- **Connection Drops**: Check distance and interference
- **Settings Not Applied**: Verify ESP32 firmware is working

## 📞 **Support**

If you encounter issues:

1. **Simulator/Emulator Issues**: These are expected limitations
2. **Real Device Issues**: Check the troubleshooting guide
3. **ESP32 Issues**: Verify firmware and hardware connections
4. **App Issues**: Check the main documentation

---

**Remember**: Simulators and emulators are great for UI development, but BLE testing requires a real device!
