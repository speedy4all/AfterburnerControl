import bleManager from './bleManager';
import { BLE_UUIDS, DEFAULT_VALUES } from './uuids';

// Hardware types
export enum HardwareType {
  LEGACY = 1, // Pixel LEDs with configurable numLeds
  NEW = 2, // 36 LED circle with 4 MOSFET channels
}

// Types for the afterburner settings
export interface AfterburnerSettings {
  mode: number;
  startColor: { r: number; g: number; b: number }; // Base intensity
  endColor: { r: number; g: number; b: number }; // Afterburner intensity
  speedMs: number;
  brightness: number;
  numLeds: number; // Kept for backward compatibility (ignored for new hardware)
  abThreshold: number;
}

export interface DeviceStatus {
  throttle: number;
  mode: number;
}

// Hardware info interface
export interface HardwareInfo {
  type: HardwareType;
  numLeds?: number; // For legacy hardware
  numChannels?: number; // For new hardware (always 4)
  totalLeds?: number; // For new hardware (always 36)
}

// Global status callback for UI updates
let statusCallback: ((status: DeviceStatus) => void) | null = null;

// Global calibration status callback for UI updates
let calibrationStatusCallback: ((status: {isCalibrated: boolean, min: number, max: number, minVisits?: number, maxVisits?: number}) => void) | null = null;

// Set status callback for UI updates
export function setStatusCallback(callback: (status: DeviceStatus) => void): void {
  statusCallback = callback;
}

// Clear status callback
export function clearStatusCallback(): void {
  statusCallback = null;
}

// Set calibration status callback for UI updates
export function setCalibrationStatusCallback(callback: (status: {
  isCalibrated: boolean, min: number, max: number, minVisits?: number, maxVisits?: number
}) => void): void {
  calibrationStatusCallback = callback;
}

// Clear calibration status callback
export function clearCalibrationStatusCallback(): void {
  calibrationStatusCallback = null;
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
    
    // Discover services and characteristics
    await bleManager.discoverServices(device.id);
    
    // Start status monitoring to keep connection alive
    try {
      await bleManager.startStatusMonitoring((status) => {
        // Parse and emit status to UI components
        if (status.error) {
          console.warn('Status update error:', status.error);
        } else if (status.throttle !== undefined && status.mode !== undefined) {
          // Valid status received - only log when there are changes
          const uiStatus = {
            throttle: status.throttle, // Already in correct format
            mode: status.mode
          };
          
          // Store the latest status for the UI to access
          // This is a simple approach - in a real app you might use a callback or event emitter
          // console.log('UI status ready:', uiStatus); // Reduced logging
          
          // Call the UI callback if it's set
          if (statusCallback) {
            statusCallback(uiStatus);
          }
        }
      });
      
      // Start calibration status monitoring
      try {
        await startCalibrationStatusMonitoring();
      } catch (error) {
        console.warn('Failed to start calibration status monitoring:', error);
        // Continue anyway - calibration should still work
      }
      
      // Start connection keep-alive
      bleManager.startConnectionKeepAlive();
    } catch (error) {
      console.warn('Failed to start status monitoring:', error);
      // Continue anyway - connection should still work
    }
    
    return true;
  } catch (error) {
    console.error('Failed to connect to Afterburner:', error);
    return false;
  }
}

// Disconnect from device
export async function disconnectFromAfterburner(): Promise<void> {
  // Stop keep-alive and monitoring before disconnecting
  bleManager.stopConnectionKeepAlive();
  await bleManager.disconnect();
}



// Write mode setting
export async function writeMode(mode: number): Promise<void> {
  try {
    await bleManager.writeCharacteristic(BLE_UUIDS.MODE, [mode]);
  } catch (error) {
    console.error('Failed to write mode:', error);
    throw error;
  }
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
    // Add a small delay to ensure BLE connection is stable
    await new Promise(resolve => setTimeout(resolve, 500));
    
    // Read each characteristic with individual error handling
    let mode = DEFAULT_VALUES.mode;
    try {
      const modeBytes = await bleManager.readCharacteristic(BLE_UUIDS.MODE);
      if (modeBytes.length > 0) {
        mode = modeBytes[0];
      }
    } catch (error) {
      console.warn('Failed to read mode:', error);
    }

    let startColor = DEFAULT_VALUES.startColor;
    try {
      const startColorBytes = await bleManager.readCharacteristic(BLE_UUIDS.START_COLOR);
      if (startColorBytes.length >= 3) {
        startColor = { r: startColorBytes[0], g: startColorBytes[1], b: startColorBytes[2] };
      }
    } catch (error) {
      console.warn('Failed to read start color:', error);
    }

    let endColor = DEFAULT_VALUES.endColor;
    try {
      const endColorBytes = await bleManager.readCharacteristic(BLE_UUIDS.END_COLOR);
      if (endColorBytes.length >= 3) {
        endColor = { r: endColorBytes[0], g: endColorBytes[1], b: endColorBytes[2] };
      }
    } catch (error) {
      console.warn('Failed to read end color:', error);
    }

    let speedMs = DEFAULT_VALUES.speedMs;
    try {
      const speedMsBytes = await bleManager.readCharacteristic(BLE_UUIDS.SPEED_MS);
      if (speedMsBytes.length >= 2) {
        speedMs = speedMsBytes[0] | (speedMsBytes[1] << 8); // little-endian
      }
    } catch (error) {
      console.warn('Failed to read speed:', error);
    }

    let brightness = DEFAULT_VALUES.brightness;
    try {
      const brightnessBytes = await bleManager.readCharacteristic(BLE_UUIDS.BRIGHTNESS);
      if (brightnessBytes.length > 0) {
        brightness = brightnessBytes[0];
      }
    } catch (error) {
      console.warn('Failed to read brightness:', error);
    }

    let numLeds = DEFAULT_VALUES.numLeds;
    try {
      const numLedsBytes = await bleManager.readCharacteristic(BLE_UUIDS.NUM_LEDS);
      if (numLedsBytes.length >= 2) {
        numLeds = numLedsBytes[0] | (numLedsBytes[1] << 8); // little-endian
      }
    } catch (error) {
      console.warn('Failed to read num LEDs:', error);
    }

    let abThreshold = DEFAULT_VALUES.abThreshold;
    try {
      const abThresholdBytes = await bleManager.readCharacteristic(BLE_UUIDS.AB_THRESHOLD);
      if (abThresholdBytes.length > 0) {
        abThreshold = abThresholdBytes[0];
      }
    } catch (error) {
      console.warn('Failed to read AB threshold:', error);
    }


    return {
      mode,
      startColor,
      endColor,
      speedMs,
      brightness,
      numLeds,
      abThreshold,
    };
  } catch (error) {
    console.error('Failed to read settings:', error);
    // Return default values if read fails
    return DEFAULT_VALUES;
  }
}

// Detect hardware type on connection
export async function detectHardwareType(): Promise<HardwareType> {
  try {
    // Try to read hardware version characteristic (if exists)
    try {
      const versionData = await bleManager.readCharacteristic(BLE_UUIDS.HARDWARE_VERSION);
      if (versionData && versionData.length > 0) {
        const version = versionData[0];
        return version === 2 ? HardwareType.NEW : HardwareType.LEGACY;
      }
    } catch (error) {
      // Hardware version characteristic doesn't exist, try fallback
      console.log('Hardware version characteristic not available, using fallback detection');
    }

    // Fallback: Try to read NUM_LEDS characteristic
    try {
      const numLedsBytes = await bleManager.readCharacteristic(BLE_UUIDS.NUM_LEDS);
      if (numLedsBytes && numLedsBytes.length >= 2) {
        // eslint-disable-next-line no-bitwise
        const numLeds = numLedsBytes[0] | (numLedsBytes[1] << 8);
        // New hardware returns 36 or 0, legacy returns configurable value (1-300)
        if (numLeds === 36 || numLeds === 0) {
          return HardwareType.NEW;
        } else if (numLeds >= 1 && numLeds <= 300) {
          return HardwareType.LEGACY;
        }
      }
    } catch (error) {
      // If NUM_LEDS read fails, assume new hardware (graceful degradation)
      console.warn('Could not detect hardware type, assuming NEW hardware');
      return HardwareType.NEW;
    }

    // Default to new hardware if detection fails
    return HardwareType.NEW;
  } catch (error) {
    console.error('Hardware type detection failed:', error);
    return HardwareType.NEW; // Default to new hardware
  }
}

// Get hardware info
export async function getHardwareInfo(): Promise<HardwareInfo> {
  const type = await detectHardwareType();

  if (type === HardwareType.LEGACY) {
    try {
      const numLedsBytes = await bleManager.readCharacteristic(BLE_UUIDS.NUM_LEDS);
      // eslint-disable-next-line no-bitwise
      const numLeds = numLedsBytes[0] | (numLedsBytes[1] << 8);
      return { type, numLeds };
    } catch (error) {
      return { type, numLeds: 45 }; // Default
    }
  } else {
    return { type, numChannels: 4, totalLeds: 36 };
  }
}

// Push all settings to device
export async function pushAllSettings(
  settings: AfterburnerSettings,
  hardwareType?: HardwareType
): Promise<void> {
  // If hardware type not provided, detect it
  if (!hardwareType) {
    hardwareType = await detectHardwareType();
  }

  // Write all settings
  await writeMode(settings.mode);
  await writeStartColor(settings.startColor);
  await writeEndColor(settings.endColor);
  await writeSpeedMs(settings.speedMs);
  await writeBrightness(settings.brightness);

  // Only write numLeds for legacy hardware
  if (hardwareType === HardwareType.LEGACY) {
    await writeNumLeds(settings.numLeds);
  }

  await writeAbThreshold(settings.abThreshold);

  // Save settings to ESP32 flash memory
  await savePreset();
}

// Monitor device status
export async function monitorStatus(callback: (status: DeviceStatus) => void): Promise<() => void> {
  try {
    return await bleManager.monitorCharacteristic(BLE_UUIDS.STATUS, (data) => {
      try {
        // Validate data
        if (!data || data.length === 0) {
          console.warn('Received empty status data');
          return;
        }
        
        // Parse JSON status from UTF-8 data
        const jsonString = String.fromCharCode(...data);
        if (!jsonString || jsonString.trim() === '') {
          console.warn('Received empty JSON string from status');
          return;
        }
        
        const status = JSON.parse(jsonString) as DeviceStatus;
        
        // Validate status object
        if (typeof status.throttle === 'number' && typeof status.mode === 'number') {
          callback(status);
        } else {
          console.warn('Invalid status format:', status);
        }
      } catch (error) {
        console.error('Failed to parse status:', error, 'Raw data:', data);
      }
    });
  } catch (error) {
    console.error('Failed to setup status monitoring:', error);
    
    // Check if device is disconnected
    if (error instanceof Error && 
        (error.message.includes('disconnected') || 
         error.message.includes('BLE operation failed'))) {
      throw new Error('Device disconnected - cannot setup status monitoring');
    }
    
    throw error;
  }
}

// Check if device is connected
export function isConnected(): boolean {
  return bleManager.isConnected();
}

// Get connected device info
export function getConnectedDevice() {
  return bleManager.getConnectedDevice();
}

// Start monitoring calibration status notifications
async function startCalibrationStatusMonitoring(): Promise<void> {
  try {
    // Start monitoring the throttle calibration status characteristic
    await bleManager.monitorCharacteristic(BLE_UUIDS.THROTTLE_CALIBRATION_STATUS, (data) => {
      try {
        if (data && data.length >= 5) {
          // Parse the status data
          // Format: [isCalibrated: 1 byte, min: 2 bytes, max: 2 bytes, minVisits?: 1 byte, maxVisits?: 1 byte]
          const isCalibrated = data[0] === 1;
          const min = (data[2] << 8) | data[1]; // Little-endian 16-bit
          const max = (data[4] << 8) | data[3]; // Little-endian 16-bit
          
          // Check if this is progress data (7 bytes) or final status (5 bytes)
          let minVisits = 0;
          let maxVisits = 0;
          if (data.length >= 7) {
            minVisits = data[5];
            maxVisits = data[6];
          }
          
          // Call the UI callback if it's set
          if (calibrationStatusCallback) {
            calibrationStatusCallback({ isCalibrated, min, max, minVisits, maxVisits });
          }
        }
      } catch (error) {
        console.error('Failed to parse calibration status:', error);
      }
    });
    
    // Also read the current status immediately after subscribing
    try {
      const currentStatus = await readThrottleCalibrationStatus();
      
      // Call the UI callback with current status
      if (calibrationStatusCallback) {
        calibrationStatusCallback(currentStatus);
      }
    } catch (error) {
      console.warn('Failed to read initial calibration status after subscribing:', error);
    }
  } catch (error) {
    console.error('Failed to start calibration status monitoring:', error);
    throw error;
  }
}

// Throttle calibration functions
export async function startThrottleCalibration(): Promise<void> {
  try {
    // Write value 1 to the throttle calibration characteristic to start calibration
    const calibrationData = [1];
    await bleManager.writeCharacteristic(BLE_UUIDS.THROTTLE_CALIBRATION, calibrationData);
  } catch (error) {
    console.error('Failed to start throttle calibration:', error);
    throw new Error('Failed to start throttle calibration');
  }
}

export async function readThrottleCalibrationStatus(): Promise<{isCalibrated: boolean, min: number, max: number}> {
  try {
    // Read from the throttle calibration status characteristic
    const data = await bleManager.readCharacteristic(BLE_UUIDS.THROTTLE_CALIBRATION_STATUS);
    
    if (data && data.length >= 5) {
      // Parse the status data
      // Format: [isCalibrated: 1 byte, min: 2 bytes, max: 2 bytes]
      const isCalibrated = data[0] === 1;
      const min = (data[2] << 8) | data[1]; // Little-endian 16-bit (consistent with notification parsing)
      const max = (data[4] << 8) | data[3]; // Little-endian 16-bit (consistent with notification parsing)
      
      return { isCalibrated, min, max };
    } else {
      return { isCalibrated: false, min: 900, max: 2000 };
    }
  } catch (error) {
    console.error('Failed to read throttle calibration status:', error);
    // Return default values on error
    return { isCalibrated: false, min: 900, max: 2000 };
  }
}

export async function resetThrottleCalibration(): Promise<void> {
  try {
    // Write value 1 to the throttle calibration reset characteristic
    const resetData = [1];
    await bleManager.writeCharacteristic(BLE_UUIDS.THROTTLE_CALIBRATION_RESET, resetData);
  } catch (error) {
    console.error('Failed to reset throttle calibration:', error);
    throw new Error('Failed to reset throttle calibration');
  }
}
