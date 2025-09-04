// Mock react-native-ble-manager
jest.mock('react-native-ble-manager', () => ({
  start: jest.fn(),
  scan: jest.fn(),
  stopScan: jest.fn(),
  connect: jest.fn(),
  disconnect: jest.fn(),
  retrieveServices: jest.fn(),
  read: jest.fn(),
  write: jest.fn(),
  startNotification: jest.fn(),
  stopNotification: jest.fn(),
  getState: jest.fn(),
}));

// Mock @react-native-community/slider
jest.mock('@react-native-community/slider', () => 'Slider');

// Mock react-native-safe-area-context
jest.mock('react-native-safe-area-context', () => ({
  useSafeAreaInsets: () => ({ top: 0, right: 0, bottom: 0, left: 0 }),
  SafeAreaProvider: ({ children }: any) => children,
}));

// Mock React Native with a simple implementation
jest.mock('react-native', () => ({
  Platform: {
    OS: 'ios',
    select: jest.fn((obj) => obj.ios || obj.default),
  },
  Dimensions: {
    get: jest.fn(() => ({ width: 375, height: 667 })),
  },
  View: 'View',
  Text: 'Text',
  TouchableOpacity: 'TouchableOpacity',
  Modal: 'Modal',
  ScrollView: 'ScrollView',
  StyleSheet: {
    create: jest.fn((styles) => styles),
  },
  Alert: {
    alert: jest.fn(),
  },
  TextInput: 'TextInput',
}));

// Mock console methods
const originalConsole = console;
beforeAll(() => {
  global.console = {
    ...originalConsole,
    log: jest.fn(),
    warn: jest.fn(),
    error: jest.fn(),
  };
});

afterAll(() => {
  global.console = originalConsole;
});

// Mock __DEV__ global
(global as any).__DEV__ = true;
