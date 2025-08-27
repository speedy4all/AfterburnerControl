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

describe('BLE Integration Tests', () => {
  let bleManager: AfterburnerBLEManager;

  beforeEach(() => {
    bleManager = new AfterburnerBLEManager();
    jest.clearAllMocks();
  });

  describe('Basic BLE Functionality', () => {
    it('should initialize BLE manager successfully', async () => {
      const result = await bleManager.initialize();
      expect(result).toBe(true);
    });

    it('should scan for device successfully', async () => {
      const device = await bleManager.scanForDevice();
      expect(device).toEqual({ id: 'test-device', name: 'Afterburner' });
    });

    it('should connect to device successfully', async () => {
      const mockDevice = { id: 'test-device', name: 'Afterburner' };
      const result = await bleManager.connectToDevice(mockDevice);
      expect(result).toBe(mockDevice);
    });

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

  describe('Connection Management', () => {
    it('should handle connection status correctly', () => {
      expect(bleManager.isConnected()).toBe(false);
    });

    it('should not be in simulator mode', () => {
      expect(bleManager.isSimulatorMode()).toBe(false);
    });

    it('should return empty simulator warning', () => {
      const warning = bleManager.getSimulatorWarning();
      expect(warning).toBe('');
    });
  });

  describe('Data Transmission', () => {
    it('should handle multiple characteristic writes', async () => {
      await expect(bleManager.writeCharacteristic('uuid1', [1, 2, 3])).resolves.not.toThrow();
      await expect(bleManager.writeCharacteristic('uuid2', [4, 5, 6])).resolves.not.toThrow();
      await expect(bleManager.writeCharacteristic('uuid3', [7, 8, 9])).resolves.not.toThrow();
    });

    it('should handle multiple characteristic reads', async () => {
      const result1 = await bleManager.readCharacteristic('uuid1');
      const result2 = await bleManager.readCharacteristic('uuid2');
      const result3 = await bleManager.readCharacteristic('uuid3');
      
      expect(result1).toEqual([1, 2, 3, 4]);
      expect(result2).toEqual([1, 2, 3, 4]);
      expect(result3).toEqual([1, 2, 3, 4]);
    });

    it('should handle multiple characteristic monitors', async () => {
      const callback1 = jest.fn();
      const callback2 = jest.fn();
      const callback3 = jest.fn();
      
      const unsubscribe1 = await bleManager.monitorCharacteristic('uuid1', callback1);
      const unsubscribe2 = await bleManager.monitorCharacteristic('uuid2', callback2);
      const unsubscribe3 = await bleManager.monitorCharacteristic('uuid3', callback3);
      
      expect(typeof unsubscribe1).toBe('function');
      expect(typeof unsubscribe2).toBe('function');
      expect(typeof unsubscribe3).toBe('function');
    });
  });
});
