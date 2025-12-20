// BLE Service and Characteristic UUIDs for ESP32 Afterburner
// Based on the specification in esp_32_afterburner_ble_react_native_starter.md

export const BLE_UUIDS = {
  // Service UUID
  SERVICE: 'b5f9a000-2b6c-4f6a-93b1-2f1f5f9ab000',
  
  // Characteristic UUIDs
  MODE: 'b5f9a001-2b6c-4f6a-93b1-2f1f5f9ab001',
  START_COLOR: 'b5f9a002-2b6c-4f6a-93b1-2f1f5f9ab002',
  END_COLOR: 'b5f9a003-2b6c-4f6a-93b1-2f1f5f9ab003',
  SPEED_MS: 'b5f9a004-2b6c-4f6a-93b1-2f1f5f9ab004',
  BRIGHTNESS: 'b5f9a005-2b6c-4f6a-93b1-2f1f5f9ab005',
  NUM_LEDS: 'b5f9a006-2b6c-4f6a-93b1-2f1f5f9ab006',
  AB_THRESHOLD: 'b5f9a007-2b6c-4f6a-93b1-2f1f5f9ab007',
  SAVE_PRESET: 'b5f9a008-2b6c-4f6a-93b1-2f1f5f9ab008',
  STATUS: 'b5f9a009-2b6c-4f6a-93b1-2f1f5f9ab009',
  
  // Throttle calibration UUIDs
  THROTTLE_CALIBRATION: 'b5f9a010-2b6c-4f6a-93b1-2f1f5f9ab010',
  THROTTLE_CALIBRATION_STATUS: 'b5f9a011-2b6c-4f6a-93b1-2f1f5f9ab011',
  THROTTLE_CALIBRATION_RESET: 'b5f9a012-2b6c-4f6a-93b1-2f1f5f9ab012',
  
  // Hardware version UUID (optional, for auto-detection)
  HARDWARE_VERSION: 'b5f9a013-2b6c-4f6a-93b1-2f1f5f9ab013',
};

// Device name to scan for
export const DEVICE_NAME = 'ABurner';

// Default values for testing
export const DEFAULT_VALUES = {
  mode: 1, // Ease
  startColor: { r: 255, g: 100, b: 0 }, // ember
  endColor: { r: 154, g: 0, b: 255 }, // purple
  speedMs: 1200,
  brightness: 200,
  numLeds: 45,
  abThreshold: 80,
};

// Mode options
export const MODES = [
  { id: 0, name: 'Linear' },
  { id: 1, name: 'Ease' },
  { id: 2, name: 'Pulse' },
];
