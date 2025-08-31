import { websocketManager, AfterburnerSettings, DeviceStatus } from './websocketManager';

// Default values for settings
export const DEFAULT_VALUES: AfterburnerSettings = {
  mode: 0,
  startColor: { r: 255, g: 0, b: 0 }, // Red
  endColor: { r: 0, g: 0, b: 255 },   // Blue
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

// Individual setting functions removed - only use pushAllSettings for reliability

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



export async function startThrottleCalibration(): Promise<void> {
  try {
    await websocketManager.sendCommand('start_calibration');
  } catch (error) {
    console.error('Failed to start throttle calibration:', error);
    throw error;
  }
}

export async function resetThrottleCalibration(): Promise<void> {
  try {
    await websocketManager.sendCommand('reset_calibration');
  } catch (error) {
    console.error('Failed to reset throttle calibration:', error);
    throw error;
  }
}

// Export types
export type { AfterburnerSettings, DeviceStatus };
