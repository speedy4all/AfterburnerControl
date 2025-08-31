import { websocketManager, AfterburnerSettings, DeviceStatus } from './websocketManager';

// Default values for settings
export const DEFAULT_VALUES: AfterburnerSettings = {
  mode: 0,
  startColor: [255, 0, 0], // Red
  endColor: [0, 0, 255],   // Blue
  speedMs: 1200,
  brightness: 200,
  numLeds: 45,
  abThreshold: 80,
};

// Connect to Afterburner device
export async function connectToAfterburner(): Promise<boolean> {
  try {
    console.log('Attempting to connect to Afterburner device...');
    const success = await websocketManager.connectToDevice();
    console.log('Connection result:', success);
    return success;
  } catch (error) {
    console.error('Failed to connect to Afterburner:', error);
    return false;
  }
}

// Disconnect from Afterburner device
export async function disconnectFromAfterburner(): Promise<void> {
  try {
    await websocketManager.disconnect();
  } catch (error) {
    console.error('Failed to disconnect from Afterburner:', error);
  }
}

// Write mode setting
export async function writeMode(mode: number): Promise<void> {
  try {
    await websocketManager.sendSettings({ mode });
  } catch (error) {
    console.error('Failed to write mode:', error);
    throw error;
  }
}

// Write start color setting
export async function writeStartColor(color: [number, number, number]): Promise<void> {
  try {
    await websocketManager.sendSettings({ startColor: color });
  } catch (error) {
    console.error('Failed to write start color:', error);
    throw error;
  }
}

// Write end color setting
export async function writeEndColor(color: [number, number, number]): Promise<void> {
  try {
    await websocketManager.sendSettings({ endColor: color });
  } catch (error) {
    console.error('Failed to write end color:', error);
    throw error;
  }
}

// Write speed setting
export async function writeSpeedMs(speedMs: number): Promise<void> {
  try {
    await websocketManager.sendSettings({ speedMs });
  } catch (error) {
    console.error('Failed to write speed:', error);
    throw error;
  }
}

// Write brightness setting
export async function writeBrightness(brightness: number): Promise<void> {
  try {
    await websocketManager.sendSettings({ brightness });
  } catch (error) {
    console.error('Failed to write brightness:', error);
    throw error;
  }
}

// Write number of LEDs setting
export async function writeNumLeds(numLeds: number): Promise<void> {
  try {
    await websocketManager.sendSettings({ numLeds });
  } catch (error) {
    console.error('Failed to write number of LEDs:', error);
    throw error;
  }
}

// Write afterburner threshold setting
export async function writeAbThreshold(abThreshold: number): Promise<void> {
  try {
    await websocketManager.sendSettings({ abThreshold });
  } catch (error) {
    console.error('Failed to write afterburner threshold:', error);
    throw error;
  }
}

// Push all settings at once
export async function pushAllSettings(settings: AfterburnerSettings): Promise<void> {
  try {
    await websocketManager.sendSettings(settings);
  } catch (error) {
    console.error('Failed to push all settings:', error);
    throw error;
  }
}

// Save preset (same as push all settings for WebSocket)
export async function savePreset(): Promise<void> {
  try {
    // For WebSocket implementation, saving preset is the same as pushing all settings
    // The device will handle saving the settings
    console.log('Preset save requested - settings will be saved by device');
  } catch (error) {
    console.error('Failed to save preset:', error);
    throw error;
  }
}

// Read settings from device (WebSocket will receive settings on connection)
export async function readSettings(): Promise<AfterburnerSettings> {
  try {
    // For WebSocket, we return default values initially
    // The device will send current settings when connected
    return { ...DEFAULT_VALUES };
  } catch (error) {
    console.error('Failed to read settings:', error);
    return { ...DEFAULT_VALUES };
  }
}

// Monitor device status
export async function monitorStatus(callback: (status: DeviceStatus) => void): Promise<() => void> {
  try {
    const unsubscribe = await websocketManager.monitorStatus(callback);
    return unsubscribe;
  } catch (error) {
    console.error('Failed to monitor status:', error);
    throw error;
  }
}

// Monitor settings updates
export async function monitorSettings(callback: (settings: AfterburnerSettings) => void): Promise<() => void> {
  try {
    websocketManager.onSettingsUpdate(callback);
    return () => {
      // Cleanup handled by WebSocket manager
    };
  } catch (error) {
    console.error('Failed to monitor settings:', error);
    throw error;
  }
}

// Send ping for testing
export async function sendPing(): Promise<void> {
  try {
    await websocketManager.sendPing();
  } catch (error) {
    console.error('Failed to send ping:', error);
    throw error;
  }
}

// Export types
export type { AfterburnerSettings, DeviceStatus };
