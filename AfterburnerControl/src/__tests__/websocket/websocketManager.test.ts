import { AfterburnerWebSocketManager } from '../../websocket/websocketManager';

// Mock WebSocket
const mockWebSocket = {
  readyState: 1, // OPEN
  send: jest.fn(),
  close: jest.fn(),
  onopen: null as any,
  onclose: null as any,
  onerror: null as any,
  onmessage: null as any,
};

// Mock global WebSocket
global.WebSocket = jest.fn(() => mockWebSocket) as any;

// Add WebSocket constants
(global.WebSocket as any).OPEN = 1;
(global.WebSocket as any).CLOSED = 3;

// Mock Platform
jest.mock('react-native', () => ({
  Platform: {
    OS: 'ios',
  },
}));

describe('AfterburnerWebSocketManager', () => {
  let manager: AfterburnerWebSocketManager;

  beforeEach(() => {
    manager = new AfterburnerWebSocketManager();
    jest.clearAllMocks();
    // Reset mock WebSocket state
    mockWebSocket.readyState = 1; // OPEN
  });

  describe('initialization', () => {
    it('should initialize successfully', async () => {
      const result = await manager.initialize();
      expect(result).toBe(true);
    });

    it('should detect simulator mode in development', () => {
      const isSimulator = manager.isSimulatorMode();
      expect(typeof isSimulator).toBe('boolean');
    });
  });

  describe('connection', () => {
    it('should connect to device successfully', async () => {
      const connectPromise = manager.connectToDevice();
      
      // Simulate successful connection
      setTimeout(() => {
        if (mockWebSocket.onopen) {
          mockWebSocket.onopen();
        }
      }, 0);

      const result = await connectPromise;
      expect(result).toBe(true);
      
      // Wait a bit for the connection state to be updated
      await new Promise(resolve => setTimeout(resolve, 10));
      expect(manager.isConnected()).toBe(true);
    });

    it('should handle connection timeout', async () => {
      const connectPromise = manager.connectToDevice();
      
      // Don't call onopen, let it timeout
      await expect(connectPromise).rejects.toThrow('Connection timeout');
    }, 15000); // Increase timeout for this test

    it('should handle connection error', async () => {
      const connectPromise = manager.connectToDevice();
      
      // Simulate connection error
      setTimeout(() => {
        if (mockWebSocket.onerror) {
          mockWebSocket.onerror(new Error('Connection failed'));
        }
      }, 0);

      await expect(connectPromise).rejects.toThrow('Connection failed');
    });
  });

  describe('message handling', () => {
    it('should send settings successfully', async () => {
      // Connect first
      const connectPromise = manager.connectToDevice();
      setTimeout(() => {
        if (mockWebSocket.onopen) {
          mockWebSocket.onopen();
        }
      }, 0);
      await connectPromise;

      // Wait a bit for connection to be established
      await new Promise(resolve => setTimeout(resolve, 10));

      // Ensure WebSocket is in OPEN state
      mockWebSocket.readyState = WebSocket.OPEN;

      const settings = {
        mode: 1,
        startColor: [255, 0, 0] as [number, number, number],
        endColor: [0, 0, 255] as [number, number, number],
        speedMs: 1000,
        brightness: 200,
        numLeds: 45,
        abThreshold: 80,
      };

      await manager.sendSettings(settings);
      expect(mockWebSocket.send).toHaveBeenCalledWith(JSON.stringify(settings));
    });

    it('should handle status messages', async () => {
      const statusCallback = jest.fn();
      await manager.monitorStatus(statusCallback);

      // Simulate status message
      const statusMessage = {
        type: 'status',
        thr: 0.75,
        mode: 2,
      };

      // Trigger the message handler directly
      manager['handleMessage'](statusMessage);

      expect(statusCallback).toHaveBeenCalledWith({
        throttle: 0.75,
        mode: 2,
      });
    });

    it('should handle settings messages', async () => {
      const settingsCallback = jest.fn();
      manager.onSettingsUpdate(settingsCallback);

      // Simulate settings message
      const settingsMessage = {
        type: 'settings',
        mode: 1,
        startColor: [255, 0, 0],
        endColor: [0, 0, 255],
        speedMs: 1000,
        brightness: 200,
        numLeds: 45,
        abThreshold: 80,
      };

      // Trigger the message handler directly
      manager['handleMessage'](settingsMessage);

      expect(settingsCallback).toHaveBeenCalledWith({
        mode: 1,
        startColor: [255, 0, 0],
        endColor: [0, 0, 255],
        speedMs: 1000,
        brightness: 200,
        numLeds: 45,
        abThreshold: 80,
      });
    });
  });

  describe('disconnection', () => {
    it('should disconnect successfully', async () => {
      // Connect first
      const connectPromise = manager.connectToDevice();
      setTimeout(() => {
        if (mockWebSocket.onopen) {
          mockWebSocket.onopen();
        }
      }, 0);
      await connectPromise;

      await manager.disconnect();
      expect(mockWebSocket.close).toHaveBeenCalled();
      expect(manager.isConnected()).toBe(false);
    });
  });

  describe('cleanup', () => {
    it('should destroy manager properly', () => {
      // First connect to create a WebSocket instance
      manager.connectToDevice();
      setTimeout(() => {
        if (mockWebSocket.onopen) {
          mockWebSocket.onopen();
        }
      }, 0);
      
      manager.destroy();
      expect(mockWebSocket.close).toHaveBeenCalled();
    });
  });
});
