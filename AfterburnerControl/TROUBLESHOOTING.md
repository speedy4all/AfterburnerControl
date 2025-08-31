# Troubleshooting Guide

This guide helps you resolve common issues with the Afterburner Control app.

## Connection Issues

### App Crashes When Clicking "Connect"

**Symptoms:**

- App crashes immediately when tapping "Connect" button
- No error message displayed

**Possible Causes:**

1. **ESP8266 device not available**
2. **WiFi not connected to device network**
3. **WebSocket connection timeout**
4. **Network security policy blocking connection**

**Solutions:**

#### 1. Check ESP8266 Device

- Ensure the ESP8266 device is powered on
- Verify the device is running the correct firmware
- Check that the device is creating the WiFi access point

#### 2. Check WiFi Connection

- Connect your mobile device to the `Afterburner_AP` WiFi network
- Password: `afterburner123`
- Verify you have internet connectivity through the device

#### 3. Test Mode (Development)

The app includes a test mode for development that simulates a successful connection:

- In development mode (`__DEV__` is true), the app will simulate connection after 3 seconds
- Look for the "🧪 Test Mode Active" indicator
- This allows you to test the UI without a real device

#### 4. Network Security

The app is configured to allow cleartext communication to:

- `192.168.4.1` (ESP8266 device)
- `localhost` (simulator testing)
- `10.0.2.2` (Android emulator)

### Connection Timeout

**Symptoms:**

- "Connection timeout" error message
- App shows "Connecting..." for 10+ seconds

**Solutions:**

1. **Check device IP address** - Ensure ESP8266 is using `192.168.4.1`
2. **Check WebSocket port** - Ensure device is listening on port `81`
3. **Check firewall settings** - Ensure port 81 is not blocked
4. **Try test mode** - Use test mode for development without device

### Settings Not Updating

**Symptoms:**

- Connected but settings changes don't affect the device
- No response from device when sending settings

**Solutions:**

1. **Check WebSocket connection status** - Ensure connection is active
2. **Verify message format** - Settings are sent as JSON
3. **Check device logs** - Look for errors in ESP8266 serial output
4. **Test with ping** - Use the "Test Ping" button to verify communication

## Debug Information

### Enable Console Logging

The app includes extensive console logging. To view logs:

**Android:**

```bash
adb logcat | grep "AfterburnerControl"
```

**iOS:**

- Use Xcode console
- Or use `react-native log-ios`

### Common Log Messages

**Successful Connection:**

```
Connecting to WebSocket: ws://192.168.4.1:81/
WebSocket connected successfully
```

**Connection Failed:**

```
WebSocket error: [error details]
Connection failed - check if ESP8266 device is running and WiFi is connected
```

**Test Mode:**

```
Test mode: Simulating successful connection
```

## Device Setup Verification

### ESP8266 Firmware Requirements

- WiFi Access Point: `Afterburner_AP`
- Password: `afterburner123`
- IP Address: `192.168.4.1`
- WebSocket Port: `81`
- WebSocket URL: `ws://192.168.4.1:81/`

### Testing Device Connectivity

1. **Web Browser Test:**

   - Connect to ESP8266 WiFi
   - Open browser to `http://192.168.4.1`
   - Should see the device web interface

2. **WebSocket Test:**
   - Use browser developer tools
   - Try connecting to `ws://192.168.4.1:81/`
   - Should establish WebSocket connection

## Development vs Production

### Development Mode

- Test mode enabled automatically
- Simulates device connection
- Allows UI testing without real device
- Shows test mode indicator

### Production Mode

- Requires real ESP8266 device
- No test mode fallback
- Full WebSocket communication
- Network security policies enforced

## Getting Help

If you're still experiencing issues:

1. **Check the console logs** for detailed error messages
2. **Verify device setup** matches the requirements
3. **Test with web browser** to isolate WebSocket issues
4. **Use test mode** for development without device
5. **Check network connectivity** and firewall settings

## Common Error Messages

| Error                                   | Cause                    | Solution                                   |
| --------------------------------------- | ------------------------ | ------------------------------------------ |
| "Connection timeout"                    | Device not responding    | Check device power and firmware            |
| "WebSocket connection failed"           | Network/security issue   | Check WiFi and network security config     |
| "CLEARTEXT communication not permitted" | Android security policy  | Network security config should handle this |
| "Failed to parse WebSocket message"     | Invalid JSON from device | Check device firmware message format       |
