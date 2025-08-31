# WebSocket Implementation for Afterburner Control

This document describes the WebSocket implementation that replaces the BLE (Bluetooth Low Energy) functionality in the Afterburner Control mobile application.

## Overview

The mobile application has been adapted to use WebSocket communication instead of BLE to connect to the ESP8266 device. This provides several advantages:

- **Better Range**: WiFi has a longer range than BLE
- **Higher Bandwidth**: WebSocket can handle more data faster
- **Easier Debugging**: WebSocket communication is easier to debug and monitor
- **Cross-Platform**: WebSocket works consistently across different platforms

## Architecture

### Components

1. **WebSocket Manager** (`src/websocket/websocketManager.ts`)

   - Handles WebSocket connection to the ESP8266 device
   - Manages connection status and reconnection logic
   - Processes incoming messages from the device

2. **Device Interface** (`src/websocket/device.ts`)

   - Provides a clean API for device communication
   - Handles settings updates and status monitoring
   - Maintains compatibility with existing UI components

3. **Constants** (`src/websocket/constants.ts`)
   - Defines configuration values and constants
   - Contains mode definitions and message types

### Connection Flow

1. **Device Setup**: ESP8266 creates a WiFi Access Point

   - SSID: `Afterburner_AP`
   - Password: `afterburner123`
   - IP Address: `192.168.4.1`
   - WebSocket Port: `81`

2. **Mobile App Connection**:

   - Connects to the ESP8266's WiFi network
   - Establishes WebSocket connection to `ws://192.168.4.1:81/`
   - Receives current settings from device
   - Starts monitoring for status updates

3. **Communication**:
   - Settings are sent as JSON messages
   - Status updates are received every 200ms
   - Automatic reconnection on connection loss

## Message Format

### Settings Update (App → Device)

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

### Settings Response (Device → App)

```json
{
  "type": "settings",
  "mode": 0,
  "startColor": [255, 0, 0],
  "endColor": [0, 0, 255],
  "speedMs": 1200,
  "brightness": 200,
  "numLeds": 45,
  "abThreshold": 80
}
```

### Status Update (Device → App)

```json
{
  "type": "status",
  "thr": 0.75,
  "mode": 2
}
```

## Configuration

### WiFi Settings

- **SSID**: `Afterburner_AP`
- **Password**: `afterburner123`
- **IP Address**: `192.168.4.1`
- **WebSocket Port**: `81`

### Connection Settings

- **Connection Timeout**: 10 seconds
- **Max Reconnection Attempts**: 5
- **Reconnection Delay**: 2 seconds

## Usage

### Connecting to Device

1. Ensure the ESP8266 device is powered on and running the firmware
2. Connect your mobile device to the `Afterburner_AP` WiFi network
3. Open the Afterburner Control app
4. Tap "Connect" to establish WebSocket connection

### Sending Settings

Settings can be sent individually or all at once:

```typescript
// Send individual setting
await writeMode(2);

// Send all settings
await pushAllSettings(settings);
```

### Monitoring Status

The app automatically monitors device status:

```typescript
const unsubscribe = await monitorStatus(status => {
  console.log('Throttle:', status.throttle);
  console.log('Mode:', status.mode);
});
```

## Simulator Support

When running in iOS Simulator or Android Emulator, the app will:

- Connect to `ws://localhost:81/` instead of the device IP
- Display a warning message about simulator limitations
- Still function for testing UI components

## Error Handling

The WebSocket implementation includes robust error handling:

- **Connection Timeout**: Automatic retry with exponential backoff
- **Network Errors**: Graceful degradation with user feedback
- **Message Parsing**: Safe JSON parsing with fallback values
- **Reconnection**: Automatic reconnection on connection loss

## Migration from BLE

The WebSocket implementation maintains API compatibility with the previous BLE implementation:

- Same function names and signatures
- Same data structures and types
- Same UI components and interactions
- Minimal changes required in the main screen

## Testing

To test the WebSocket implementation:

1. **Real Device**: Connect to ESP8266 device via WiFi
2. **Simulator**: Use localhost for development testing
3. **Web Browser**: Access the device's web interface at `http://192.168.4.1`

## Troubleshooting

### Common Issues

1. **Connection Failed**

   - Check if device is powered on
   - Verify WiFi network connection
   - Ensure firmware is running

2. **Settings Not Updating**

   - Check WebSocket connection status
   - Verify message format
   - Check device logs for errors

3. **Status Not Receiving**
   - Verify WebSocket connection is active
   - Check device is sending status updates
   - Monitor network connectivity

### Debug Information

Enable debug logging by checking the console output:

- Connection attempts and status
- Message sending and receiving
- Error details and stack traces
