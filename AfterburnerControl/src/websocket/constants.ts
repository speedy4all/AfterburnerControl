// Mode definitions
export const MODES = [
  { id: 0, name: 'Static' },
  { id: 1, name: 'Pulse' },
  { id: 2, name: 'Afterburner' },
];

// WebSocket configuration
export const WEBSOCKET_CONFIG = {
  // ESP8266 Access Point IP address
  DEVICE_IP: '192.168.4.1',
  // WebSocket port
  PORT: 81,
  // Connection timeout in milliseconds
  CONNECTION_TIMEOUT: 10000,
  // Reconnection attempts
  MAX_RECONNECT_ATTEMPTS: 5,
  // Reconnection delay in milliseconds
  RECONNECT_DELAY: 2000,
};

// WiFi configuration
export const WIFI_CONFIG = {
  SSID: 'Afterburner_AP',
  PASSWORD: 'afterburner123',
};

// Message types
export const MESSAGE_TYPES = {
  SETTINGS: 'settings',
  STATUS: 'status',
  COMMAND: 'command',
};

// Commands
export const COMMANDS = {
  SAVE_PRESET: 'save_preset',
  RESET_SETTINGS: 'reset_settings',
  GET_STATUS: 'get_status',
};
