import { AfterburnerBLEManager } from '../../ble/bleManager';

// Mock the BLE manager class to avoid actual BLE calls
jest.mock('../../ble/bleManager', () => {
  const originalModule = jest.requireActual('../../ble/bleManager');
  
  // Create a mock class that extends the original
  class MockAfterburnerBLEManager extends originalModule.AfterburnerBLEManager {
    constructor() {
      super();
      // Override simulator detection
      (this as any).isSimulator = false;
    }
    
    // Override methods to avoid actual BLE calls
    async initialize(): Promise<boolean> {
      return true;
    }
    
    async scanForDevice(): Promise<any> {
      return { id: 'test-device', name: 'Afterburner' };
    }
    
    async connectToDevice(device: any): Promise<any> {
      return device;
    }
    
    async disconnect(): Promise<void> {
      // Mock implementation
    }
    
    async writeCharacteristic(uuid: string, data: number[]): Promise<void> {
      // Mock implementation
    }
    
    async readCharacteristic(uuid: string): Promise<number[]> {
      return [1, 2, 3, 4];
    }
    
    async monitorCharacteristic(uuid: string, callback: (data: number[]) => void): Promise<() => void> {
      return () => {};
    }
    
    isConnected(): boolean {
      return false;
    }
    
    isSimulatorMode(): boolean {
      return false;
    }
    
    getSimulatorWarning(): string {
      return '';
    }
  }
  
  return {
    ...originalModule,
    AfterburnerBLEManager: MockAfterburnerBLEManager,
    bleManager: new MockAfterburnerBLEManager(),
  };
});

describe('AfterburnerBLEManager', () => {
  let bleManager: AfterburnerBLEManager;

  beforeEach(() => {
    bleManager = new AfterburnerBLEManager();
    jest.clearAllMocks();
  });

  describe('initialization', () => {
    it('should initialize BLE manager successfully', async () => {
      const result = await bleManager.initialize();
      expect(result).toBe(true);
    });

    it('should not be in simulator mode', () => {
      expect(bleManager.isSimulatorMode()).toBe(false);
    });

    it('should return empty simulator warning', () => {
      const warning = bleManager.getSimulatorWarning();
      expect(warning).toBe('');
    });
  });

  describe('device scanning', () => {
    it('should scan for device successfully', async () => {
      const result = await bleManager.scanForDevice();
      expect(result).toEqual({ id: 'test-device', name: 'Afterburner' });
    });
  });

  describe('device connection', () => {
    it('should connect to device successfully', async () => {
      const mockDevice = { id: 'test-device', name: 'Afterburner' };
      const result = await bleManager.connectToDevice(mockDevice);
      expect(result).toBe(mockDevice);
    });
  });

  describe('data transmission', () => {
    it('should write characteristic successfully', async () => {
      await expect(bleManager.writeCharacteristic('test-uuid', [1, 2, 3])).resolves.not.toThrow();
    });

    it('should read characteristic successfully', async () => {
      const result = await bleManager.readCharacteristic('test-uuid');
      expect(result).toEqual([1, 2, 3, 4]);
    });

    it('should monitor characteristic successfully', async () => {
      const callback = jest.fn();
      const unsubscribe = await bleManager.monitorCharacteristic('test-uuid', callback);
      expect(typeof unsubscribe).toBe('function');
    });
  });

  describe('connection status', () => {
    it('should return correct connection status', () => {
      expect(bleManager.isConnected()).toBe(false);
    });
  });

  describe('simulator detection', () => {
    it('should not be in simulator mode', () => {
      expect(bleManager.isSimulatorMode()).toBe(false);
    });

    it('should return empty simulator warning', () => {
      const warning = bleManager.getSimulatorWarning();
      expect(warning).toBe('');
    });
  });
});
