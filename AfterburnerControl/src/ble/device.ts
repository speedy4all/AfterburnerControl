import { bleManager } from './bleManager';
import { BLE_UUIDS, DEFAULT_VALUES } from './uuids';

// Types for the afterburner settings
export interface AfterburnerSettings {
  mode: number;
  startColor: { r: number; g: number; b: number };
  endColor: { r: number; g: number; b: number };
  speedMs: number;
  brightness: number;
  numLeds: number;
  abThreshold: number;
}

export interface DeviceStatus {
  throttle: number;
  mode: number;
}

// Helper function to convert RGB color to byte array
function colorToBytes(color: { r: number; g: number; b: number }): number[] {
  return [color.r, color.g, color.b];
}

// Helper function to convert number to little-endian bytes
function numberToBytes(value: number, byteCount: number): number[] {
  const bytes: number[] = [];
  for (let i = 0; i < byteCount; i++) {
    bytes.push((value >> (i * 8)) & 0xff);
  }
  return bytes;
}

// Connect to Afterburner device
export async function connectToAfterburner(): Promise<boolean> {
  try {
    // Initialize BLE
    const initialized = await bleManager.initialize();
    if (!initialized) {
      throw new Error('Failed to initialize BLE');
    }

    // Scan for device
    const device = await bleManager.scanForDevice();
    if (!device) {
      throw new Error('Afterburner device not found');
    }

    // Connect to device
    await bleManager.connectToDevice(device);
    return true;
  } catch (error) {
    console.error('Failed to connect to Afterburner:', error);
    return false;
  }
}

// Disconnect from device
export async function disconnectFromAfterburner(): Promise<void> {
  await bleManager.disconnect();
}

// Write mode setting
export async function writeMode(mode: number): Promise<void> {
  await bleManager.writeCharacteristic(BLE_UUIDS.MODE, [mode]);
}

// Write start color
export async function writeStartColor(color: { r: number; g: number; b: number }): Promise<void> {
  await bleManager.writeCharacteristic(BLE_UUIDS.START_COLOR, colorToBytes(color));
}

// Write end color
export async function writeEndColor(color: { r: number; g: number; b: number }): Promise<void> {
  await bleManager.writeCharacteristic(BLE_UUIDS.END_COLOR, colorToBytes(color));
}

// Write speed setting
export async function writeSpeedMs(speedMs: number): Promise<void> {
  await bleManager.writeCharacteristic(BLE_UUIDS.SPEED_MS, numberToBytes(speedMs, 2));
}

// Write brightness setting
export async function writeBrightness(brightness: number): Promise<void> {
  await bleManager.writeCharacteristic(BLE_UUIDS.BRIGHTNESS, [brightness]);
}

// Write number of LEDs
export async function writeNumLeds(numLeds: number): Promise<void> {
  await bleManager.writeCharacteristic(BLE_UUIDS.NUM_LEDS, numberToBytes(numLeds, 2));
}

// Write afterburner threshold
export async function writeAbThreshold(threshold: number): Promise<void> {
  await bleManager.writeCharacteristic(BLE_UUIDS.AB_THRESHOLD, [threshold]);
}

// Save preset to device
export async function savePreset(): Promise<void> {
  await bleManager.writeCharacteristic(BLE_UUIDS.SAVE_PRESET, [1]);
}

// Read current settings from device
export async function readSettings(): Promise<AfterburnerSettings> {
  try {
    const mode = (await bleManager.readCharacteristic(BLE_UUIDS.MODE))[0];
    const startColorBytes = await bleManager.readCharacteristic(BLE_UUIDS.START_COLOR);
    const endColorBytes = await bleManager.readCharacteristic(BLE_UUIDS.END_COLOR);
    const speedMsBytes = await bleManager.readCharacteristic(BLE_UUIDS.SPEED_MS);
    const brightness = (await bleManager.readCharacteristic(BLE_UUIDS.BRIGHTNESS))[0];
    const numLedsBytes = await bleManager.readCharacteristic(BLE_UUIDS.NUM_LEDS);
    const abThreshold = (await bleManager.readCharacteristic(BLE_UUIDS.AB_THRESHOLD))[0];

    return {
      mode,
      startColor: { r: startColorBytes[0], g: startColorBytes[1], b: startColorBytes[2] },
      endColor: { r: endColorBytes[0], g: endColorBytes[1], b: endColorBytes[2] },
      speedMs: speedMsBytes[0] | (speedMsBytes[1] << 8), // little-endian
      brightness,
      numLeds: numLedsBytes[0] | (numLedsBytes[1] << 8), // little-endian
      abThreshold,
    };
  } catch (error) {
    console.error('Failed to read settings:', error);
    // Return default values if read fails
    return DEFAULT_VALUES;
  }
}

// Push all settings to device
export async function pushAllSettings(settings: AfterburnerSettings): Promise<void> {
  await writeMode(settings.mode);
  await writeStartColor(settings.startColor);
  await writeEndColor(settings.endColor);
  await writeSpeedMs(settings.speedMs);
  await writeBrightness(settings.brightness);
  await writeNumLeds(settings.numLeds);
  await writeAbThreshold(settings.abThreshold);
}

// Monitor device status
export async function monitorStatus(callback: (status: DeviceStatus) => void): Promise<() => void> {
  return await bleManager.monitorCharacteristic(BLE_UUIDS.STATUS, (data) => {
    try {
      // Parse JSON status from UTF-8 data
      const jsonString = String.fromCharCode(...data);
      const status = JSON.parse(jsonString) as DeviceStatus;
      callback(status);
    } catch (error) {
      console.error('Failed to parse status:', error);
    }
  });
}

// Check if device is connected
export function isConnected(): boolean {
  return bleManager.isConnected();
}

// Get connected device info
export function getConnectedDevice() {
  return bleManager.getConnectedDevice();
}
