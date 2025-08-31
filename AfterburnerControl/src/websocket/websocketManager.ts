import { Platform } from 'react-native';

// WebSocket message types
export interface WebSocketMessage {
  type: string;
  [key: string]: any;
}

export interface AfterburnerSettings {
  mode: number;
  startColor: { r: number; g: number; b: number };
  endColor: { r: number; g: number; b: number };
  speedMs: number;
  brightness: number;
  numLeds: number;
  abThreshold: number;
}

export interface DeviceStatus {
  throttle: number;
  mode: number;
  signalValid: boolean;
  pulseCount: number;
  invalidPulseCount: number;
  calibrating: boolean;
  calibrationComplete: boolean;
  minPulse?: number;
  maxPulse?: number;
  pulseRange?: number;
}

export interface ConnectionStatus {
  connected: boolean;
  connecting: boolean;
  error?: string;
}

class AfterburnerWebSocketManager {
  private ws: WebSocket | null = null;
  private reconnectAttempts = 0;
  private maxReconnectAttempts = 5;
  private reconnectDelay = 2000;
  private isSimulator = false;
  private enableTestMode = false;
  private statusCallback: ((status: DeviceStatus) => void) | null = null;
  private connectionCallback: ((status: ConnectionStatus) => void) | null = null;
  private settingsCallback: ((settings: AfterburnerSettings) => void) | null = null;

  constructor() {
    // For now, we'll assume we're not in a simulator unless explicitly detected
    // This ensures real devices always try to connect to the ESP8266
    this.isSimulator = false;
    
    // For development/testing, we can enable a test mode
    // But disable it for now to avoid development bridge issues
    this.enableTestMode = false; // __DEV__;
  }

  // Initialize WebSocket manager
  async initialize(): Promise<boolean> {
    try {
      // Handle simulator/emulator limitation
      if (this.isSimulator) {
        console.log('Running in Simulator/Emulator - WebSocket will use localhost');
        return true;
      }

      return true;
    } catch (error) {
      console.error('Failed to initialize WebSocket manager:', error);
      return false;
    }
  }

  // Connect to device
  async connectToDevice(): Promise<boolean> {
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      console.log('Already connected');
      return true;
    }

    try {
      this.updateConnectionStatus({ connected: false, connecting: true });
      
      const wsUrl = this.isSimulator 
        ? 'ws://localhost:81/' 
        : 'ws://192.168.4.1:81/'; // ESP8266 AP IP address

      console.log('Connecting to WebSocket:', wsUrl);
      console.log('Device detection - isSimulator:', this.isSimulator, 'enableTestMode:', this.enableTestMode);
      
      // Check if WebSocket is available
      if (typeof WebSocket === 'undefined') {
        throw new Error('WebSocket is not available in this environment');
      }
      
      // Add error handling for WebSocket constructor
      try {
        this.ws = new WebSocket(wsUrl);
      } catch (wsError) {
        console.error('Failed to create WebSocket:', wsError);
        throw new Error('Failed to create WebSocket connection');
      }

      return new Promise((resolve, reject) => {
        const timeout = setTimeout(() => {
          reject(new Error('Connection timeout'));
        }, 10000);

        this.ws!.onopen = () => {
          console.log('WebSocket connected successfully');
          clearTimeout(timeout);
          this.reconnectAttempts = 0;
          this.updateConnectionStatus({ connected: true, connecting: false });
          resolve(true);
        };

        // For development, if connection fails, we can simulate a successful connection
        if (this.enableTestMode) {
          setTimeout(() => {
            if (!this.ws || this.ws.readyState !== WebSocket.OPEN) {
              console.log('Test mode: Simulating successful connection');
              clearTimeout(timeout);
              this.updateConnectionStatus({ connected: true, connecting: false });
              resolve(true);
            }
          }, 3000); // Wait 3 seconds before simulating connection
        }

        // Add additional debugging for real device connections
        if (!this.isSimulator) {
          console.log('Real device connection - will attempt to connect to ESP8266');
          console.log('WebSocket readyState:', this.ws.readyState);
        }

        this.ws!.onclose = (event) => {
          console.log('WebSocket disconnected:', event.code, event.reason);
          this.updateConnectionStatus({ 
            connected: false, 
            connecting: false, 
            error: `Disconnected: ${event.reason || 'Unknown reason'}` 
          });
          
          if (this.reconnectAttempts < this.maxReconnectAttempts) {
            this.reconnectAttempts++;
            console.log(`Attempting to reconnect (${this.reconnectAttempts}/${this.maxReconnectAttempts})...`);
            setTimeout(() => {
              this.connectToDevice();
            }, this.reconnectDelay);
          }
        };

        this.ws!.onerror = (error) => {
          console.error('WebSocket error:', error);
          clearTimeout(timeout);
          this.updateConnectionStatus({ 
            connected: false, 
            connecting: false, 
            error: 'Connection failed - check if ESP8266 device is running and WiFi is connected' 
          });
          reject(new Error('WebSocket connection failed'));
        };

        // Add error handling for onmessage to prevent crashes
        this.ws!.onmessage = (event) => {
          try {
            console.log('WebSocket message received:', event.data);
            
            try {
              const data: WebSocketMessage = JSON.parse(event.data);
              this.handleMessage(data);
            } catch (error) {
              console.error('Failed to parse WebSocket message:', error);
            }
          } catch (error) {
            console.error('Error handling WebSocket message:', error);
          }
        };
      });
    } catch (error) {
      console.error('Connection failed:', error);
      this.updateConnectionStatus({ 
        connected: false, 
        connecting: false, 
        error: error instanceof Error ? error.message : 'Connection failed' 
      });
      return false;
    }
  }

  // Disconnect from device
  async disconnect(): Promise<void> {
    if (this.ws) {
      this.ws.close();
      this.ws = null;
      this.updateConnectionStatus({ connected: false, connecting: false });
      console.log('WebSocket disconnected');
    }
  }

  // Send settings to device
  async sendSettings(settings: AfterburnerSettings): Promise<void> {
    if (!this.ws || this.ws.readyState !== WebSocket.OPEN) {
      throw new Error('WebSocket not connected');
    }

    try {
      // Convert color objects to arrays for the device
      const deviceSettings: any = { ...settings };
      deviceSettings.startColor = [settings.startColor.r, settings.startColor.g, settings.startColor.b];
      deviceSettings.endColor = [settings.endColor.r, settings.endColor.g, settings.endColor.b];
      
      const message = JSON.stringify(deviceSettings);
      console.log('Sending JSON message:', message);
      this.ws.send(message);
      console.log('Settings sent:', deviceSettings);
    } catch (error) {
      console.error('Failed to send settings:', error);
      throw error;
    }
  }



  // Send command to device
  async sendCommand(command: string): Promise<void> {
    if (!this.ws || this.ws.readyState !== WebSocket.OPEN) {
      throw new Error('WebSocket not connected');
    }

    try {
      const message = JSON.stringify({ type: 'command', command });
      console.log('Sending command:', message);
      this.ws.send(message);
    } catch (error) {
      console.error('Failed to send command:', error);
      throw error;
    }
  }

  // Monitor device status
  async monitorStatus(callback: (status: DeviceStatus) => void): Promise<() => void> {
    this.statusCallback = callback;
    
    return () => {
      this.statusCallback = null;
    };
  }

  // Monitor connection status
  onConnectionStatusChange(callback: (status: ConnectionStatus) => void): void {
    this.connectionCallback = callback;
  }

  // Monitor settings updates
  onSettingsUpdate(callback: (settings: AfterburnerSettings) => void): void {
    this.settingsCallback = callback;
  }

  // Handle incoming messages
  private handleMessage(data: WebSocketMessage): void {
    console.log('Received message:', data);

    if (data.type === 'status' && this.statusCallback) {
      const status: DeviceStatus = {
        throttle: data.thr || 0,
        mode: data.mode || 0,
        signalValid: data.signalValid || false,
        pulseCount: data.pulseCount || 0,
        invalidPulseCount: data.invalidPulseCount || 0,
        calibrating: data.calibrating || false,
        calibrationComplete: data.calibrationComplete || false,
        minPulse: data.minPulse,
        maxPulse: data.maxPulse,
        pulseRange: data.pulseRange
      };
      console.log('Calling status callback with:', status);
      this.statusCallback(status);
    } else if (data.type === 'settings' && this.settingsCallback) {
      // Handle settings update from device
      // Convert array colors from device to object format for the mobile app
      const settings: AfterburnerSettings = {
        mode: data.mode || 0,
        startColor: Array.isArray(data.startColor) 
          ? { r: data.startColor[0] || 0, g: data.startColor[1] || 0, b: data.startColor[2] || 0 }
          : { r: 255, g: 0, b: 0 },
        endColor: Array.isArray(data.endColor)
          ? { r: data.endColor[0] || 0, g: data.endColor[1] || 0, b: data.endColor[2] || 0 }
          : { r: 0, g: 0, b: 255 },
        speedMs: data.speedMs || 1200,
        brightness: data.brightness || 200,
        numLeds: data.numLeds || 45,
        abThreshold: data.abThreshold || 80
      };
      console.log('Calling settings callback with:', settings);
      this.settingsCallback(settings);
    }
  }

  // Update connection status
  private updateConnectionStatus(status: ConnectionStatus): void {
    if (this.connectionCallback) {
      this.connectionCallback(status);
    }
  }

  // Get connection status
  isConnected(): boolean {
    return this.ws !== null && this.ws.readyState === WebSocket.OPEN;
  }

  // Get WebSocket instance (for testing)
  getWebSocket(): WebSocket | null {
    return this.ws;
  }

  // Check if running in simulator
  isSimulatorMode(): boolean {
    return this.isSimulator;
  }

  // Get simulator warning message
  getSimulatorWarning(): string {
    if (this.isSimulator) {
      return 'WebSocket will connect to localhost. Make sure the ESP8266 is running and accessible.';
    }
    return '';
  }

  // Check if test mode is enabled
  isTestMode(): boolean {
    return this.enableTestMode;
  }



  // Destroy manager
  destroy(): void {
    if (this.ws) {
      this.ws.close();
      this.ws = null;
    }
  }
}

// Export singleton instance
export const websocketManager = new AfterburnerWebSocketManager();

// Export the class for testing
export { AfterburnerWebSocketManager };
