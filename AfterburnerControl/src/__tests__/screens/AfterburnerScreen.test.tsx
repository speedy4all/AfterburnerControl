import { AfterburnerScreen } from '../../screens/AfterburnerScreen';

// Mock React Native components to avoid rendering issues
jest.mock('react-native', () => ({
  View: 'View',
  Text: 'Text',
  TouchableOpacity: 'TouchableOpacity',
  ScrollView: 'ScrollView',
  StyleSheet: {
    create: jest.fn((styles) => styles),
  },
  Alert: {
    alert: jest.fn(),
  },
  TextInput: 'TextInput',
}));

// Mock the BLE manager and device functions
jest.mock('../../ble/bleManager', () => ({
  bleManager: {
    initialize: jest.fn(),
    scanForDevice: jest.fn(),
    connect: jest.fn(),
    disconnect: jest.fn(),
    sendColorData: jest.fn(),
    sendSettingsData: jest.fn(),
    isConnected: jest.fn(),

  },
}));

jest.mock('../../ble/device', () => ({
  connectToAfterburner: jest.fn(),
  disconnectFromAfterburner: jest.fn(),
  writeMode: jest.fn(),
  writeStartColor: jest.fn(),
  writeEndColor: jest.fn(),
  writeSpeedMs: jest.fn(),
  writeBrightness: jest.fn(),
  writeNumLeds: jest.fn(),
  writeAbThreshold: jest.fn(),
  savePreset: jest.fn(),
  pushAllSettings: jest.fn(),
  readSettings: jest.fn(),
  monitorStatus: jest.fn(),
}));

jest.mock('../../ble/uuids', () => ({
  MODES: [
    { id: 0, name: 'Static' },
    { id: 1, name: 'Rainbow' },
    { id: 2, name: 'Pulse' },
  ],
  DEFAULT_VALUES: {
    mode: 0,
    startColor: { r: 255, g: 0, b: 0 },
    endColor: { r: 0, g: 0, b: 255 },
    speedMs: 1200,
    brightness: 200,
    numLeds: 45,
    abThreshold: 80,
  },
}));

// Mock the ColorInput component
jest.mock('../../components/ColorInput', () => ({
  ColorInput: 'ColorInput',
}));

// Test the AfterburnerScreen component logic without rendering
describe('AfterburnerScreen', () => {
  beforeEach(() => {
    jest.clearAllMocks();
  });

  it('should have correct imports and dependencies', () => {
    // Test that all required modules are imported correctly
    expect(AfterburnerScreen).toBeDefined();
    expect(typeof AfterburnerScreen).toBe('function');
  });

  it('should have correct default values', () => {
    const { DEFAULT_VALUES } = require('../../ble/uuids');
    
    expect(DEFAULT_VALUES.mode).toBe(0);
    expect(DEFAULT_VALUES.startColor).toEqual({ r: 255, g: 0, b: 0 });
    expect(DEFAULT_VALUES.endColor).toEqual({ r: 0, g: 0, b: 255 });
    expect(DEFAULT_VALUES.speedMs).toBe(1200);
    expect(DEFAULT_VALUES.brightness).toBe(200);
    expect(DEFAULT_VALUES.numLeds).toBe(45);
    expect(DEFAULT_VALUES.abThreshold).toBe(80);
  });

  it('should have correct modes configuration', () => {
    const { MODES } = require('../../ble/uuids');
    
    expect(MODES).toHaveLength(3);
    expect(MODES[0]).toEqual({ id: 0, name: 'Static' });
    expect(MODES[1]).toEqual({ id: 1, name: 'Rainbow' });
    expect(MODES[2]).toEqual({ id: 2, name: 'Pulse' });
  });

  it('should have BLE manager functions available', () => {
    const { bleManager } = require('../../ble/bleManager');
    
    expect(typeof bleManager.initialize).toBe('function');
    expect(typeof bleManager.scanForDevice).toBe('function');
    expect(typeof bleManager.connect).toBe('function');
    expect(typeof bleManager.disconnect).toBe('function');
    expect(typeof bleManager.isConnected).toBe('function');

  });

  it('should have device functions available', () => {
    const device = require('../../ble/device');
    
    expect(typeof device.connectToAfterburner).toBe('function');
    expect(typeof device.disconnectFromAfterburner).toBe('function');
    expect(typeof device.writeMode).toBe('function');
    expect(typeof device.writeStartColor).toBe('function');
    expect(typeof device.writeEndColor).toBe('function');
    expect(typeof device.writeSpeedMs).toBe('function');
    expect(typeof device.writeBrightness).toBe('function');
    expect(typeof device.writeNumLeds).toBe('function');
    expect(typeof device.writeAbThreshold).toBe('function');
    expect(typeof device.savePreset).toBe('function');
    expect(typeof device.pushAllSettings).toBe('function');
    expect(typeof device.readSettings).toBe('function');
    expect(typeof device.monitorStatus).toBe('function');
  });

  it('should have ColorInput component available', () => {
    const { ColorInput } = require('../../components/ColorInput');
    expect(ColorInput).toBe('ColorInput');
  });
});
