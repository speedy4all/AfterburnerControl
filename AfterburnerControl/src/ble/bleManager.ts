import BleManager from 'react-native-ble-manager';
import { NativeEventEmitter, NativeModules } from 'react-native';
import { BLE_UUIDS, DEVICE_NAME } from './uuids';

// Define BLE event types
type BLEEventType = 
  | 'BleManagerDiscoverPeripheral'
  | 'BleManagerStopScan'
  | 'BleManagerDisconnectPeripheral'
  | 'BleManagerDidUpdateValueForCharacteristic'
  | 'BleManagerDidUpdateNotificationStateFor'
  | 'BleManagerConnectPeripheral'
  | 'BleManagerFailToConnectPeripheral'
  | 'BleManagerDidUpdateState';

interface BLEDevice {
  id: string;
  name: string;
  rssi: number;
  advertising: any;
}


class AfterburnerBLEManager {
  private isInitialized = false;
  private isScanning = false;
  private connectedDevice: BLEDevice | null = null;
  private eventEmitter: NativeEventEmitter;
  private eventListeners: Array<any> = [];
  private keepAliveInterval: NodeJS.Timeout | null = null;
  private lastStatusMode: number = -1;
  private lastStatusThrottle: number = -1;
  private lastStatusLog: number = 0;
  private statusUnsubscribe: (() => void) | null = null;
  constructor() {
    this.eventEmitter = new NativeEventEmitter(NativeModules.BleManager);
  }

  // Initialize BLE manager
  async initialize(): Promise<boolean> {
    try {

      if (this.isInitialized) {
        return true;
      }

      // Start BLE manager
      BleManager.start({ showAlert: false });
      
      // Wait for BLE to be ready
      return new Promise((resolve, reject) => {
        const timeout = setTimeout(() => {
          reject(new Error('BLE initialization timeout - BLE did not become ready'));
        }, 10000); // 10 second timeout
        
        const checkState = () => {
          // react-native-ble-manager doesn't have getState, so we'll assume BLE is ready
          // after calling BleManager.start()
          clearTimeout(timeout);
          this.isInitialized = true;
          this.setupEventListeners();
          resolve(true);
        };
        
        checkState();
      });
    } catch (error) {
      console.error('Failed to initialize BLE:', error);
      return false;
    }
  }

  // Set up event listeners for BLE events
  private setupEventListeners(): void {
    const events: BLEEventType[] = [
      'BleManagerDiscoverPeripheral',
      'BleManagerStopScan',
      'BleManagerDisconnectPeripheral',
      'BleManagerDidUpdateValueForCharacteristic',
      'BleManagerDidUpdateNotificationStateFor',
      'BleManagerConnectPeripheral',
      'BleManagerFailToConnectPeripheral',
      'BleManagerDidUpdateState'
    ];

    events.forEach(eventType => {
      const listener = this.eventEmitter.addListener(eventType, (data) => {
        this.handleBLEEvent(eventType, data);
      });
      this.eventListeners.push(listener);
    });
    
    console.log('🔧 BLE event listeners setup complete');
  }

  // Handle BLE events
  private handleBLEEvent(eventType: BLEEventType, data: any): void {
    switch (eventType) {
      case 'BleManagerDiscoverPeripheral':
        break;
      case 'BleManagerStopScan':
        this.isScanning = false;
        break;
      case 'BleManagerDisconnectPeripheral':
        // Only clear connected device if it's the same device
        if (this.connectedDevice && this.connectedDevice.id === data.peripheral) {
          this.connectedDevice = null;
          
          // Stop keep-alive when device disconnects
          this.stopConnectionKeepAlive();
        }
        break;
      case 'BleManagerDidUpdateValueForCharacteristic':
        // Only log if it's NOT the status characteristic to avoid flooding
        break;
      case 'BleManagerDidUpdateNotificationStateFor':
        break;
      case 'BleManagerConnectPeripheral':
        break;
      case 'BleManagerFailToConnectPeripheral':
        break;
      case 'BleManagerDidUpdateState':
        break;
    }
  }

  // Scan for Afterburner device
  async scanForDevice(): Promise<BLEDevice | null> {
    if (this.isScanning) {
      return null;
    }

    try {
      this.isScanning = true;

      return new Promise((resolve, reject) => {
        const timeout = setTimeout(() => {
          this.isScanning = false;
          BleManager.stopScan();
          reject(new Error('Scan timeout - device not found'));
        }, 15000); // 15 second timeout

        let allDevices: any[] = [];

        // Set up discovery listener
        const discoveryListener = this.eventEmitter.addListener('BleManagerDiscoverPeripheral', (data) => {
          allDevices.push(data);
          
          if (data.name === DEVICE_NAME) {
            this.isScanning = false;
            BleManager.stopScan();
            clearTimeout(timeout);
            discoveryListener.remove();
            resolve(data);
          }
        });

        // Start scanning
        BleManager.scan([], 15, true);
      });
    } catch (error) {
      this.isScanning = false;
      console.error('Scan failed:', error);
      throw error;
    }
  }

  // Connect to device
  async connectToDevice(device: BLEDevice): Promise<BLEDevice> {
    try {
      return new Promise((resolve, reject) => {
        const timeout = setTimeout(() => {
          reject(new Error('Connection timeout'));
        }, 10000); // 10 second timeout

        const connectListener = this.eventEmitter.addListener('BleManagerConnectPeripheral', (data) => {
          if (data.peripheral === device.id) {
            clearTimeout(timeout);
            connectListener.remove();
            this.connectedDevice = device;
            resolve(device);
          }
        });

        const failListener = this.eventEmitter.addListener('BleManagerFailToConnectPeripheral', (data) => {
          if (data.peripheral === device.id) {
            clearTimeout(timeout);
            connectListener.remove();
            failListener.remove();
            reject(new Error('Failed to connect to device'));
          }
        });

        // Connect to device
        BleManager.connect(device.id);
      });
    } catch (error) {
      console.error('Connection failed:', error);
      throw error;
    }
  }

  // Discover services and characteristics
  async discoverServices(deviceId: string): Promise<void> {
    try {
      // Use react-native-ble-manager's retrieveServices method
      return new Promise((resolve, reject) => {
        BleManager.retrieveServices(deviceId).then((_peripheralInfo) => {
          resolve();
        }).catch((error) => {
          console.error('Service discovery failed:', error);
          reject(new Error(`Service discovery failed: ${error}`));
        });
      });
    } catch (error) {
      console.error('Service discovery failed:', error);
      throw error;
    }
  }

  // Read characteristic
  async readCharacteristic(characteristicUUID: string): Promise<number[]> {
    if (!this.connectedDevice) {
      throw new Error('No device connected');
    }

    try {
      return new Promise((resolve, reject) => {
        // react-native-ble-manager read method returns a Promise
        BleManager.read(
          this.connectedDevice!.id,
          BLE_UUIDS.SERVICE,
          characteristicUUID
        ).then((data) => {
          // react-native-ble-manager returns data as raw bytes, not base64
          if (Array.isArray(data)) {
            // Data is already a byte array
            resolve(data);
          } else if (typeof data === 'string') {
            // Data is a string, try to convert from base64
            try {
              const bytes = this.base64ToArray(data);
              resolve(bytes);
            } catch (error) {
              console.warn('Failed to convert from base64, treating as raw data:', error);
              // If base64 conversion fails, treat as raw string
              const bytes = Array.from(data as string).map((char: string) => char.charCodeAt(0));
              resolve(bytes);
            }
          } else {
            // Unknown data type, return empty array
            console.warn('Unknown data type from BLE read:', typeof data, data);
            resolve([]);
          }
        }).catch((error) => {
          console.error('Read characteristic failed:', error);
          reject(new Error(`Read characteristic failed: ${error}`));
        });
      });
    } catch (error) {
      console.error('Read characteristic failed:', error);
      throw error;
    }
  }

  // Write characteristic
  async writeCharacteristic(characteristicUUID: string, data: number[]): Promise<void> {
    if (!this.connectedDevice) {
      throw new Error('No device connected');
    }

    try {
      return new Promise((resolve, reject) => {
        // react-native-ble-manager write method signature: BleManager.write(peripheralId, serviceUUID, characteristicUUID, data)
        BleManager.write(
          this.connectedDevice!.id,
          BLE_UUIDS.SERVICE,
          characteristicUUID,
          data
        ).then(() => {
          resolve();
        }).catch((error) => {
          console.error(`❌ Write characteristic failed for ${characteristicUUID}:`, error);
          reject(new Error(`Write characteristic failed: ${error}`));
        });
      });
    } catch (error) {
      console.error(`❌ Write characteristic failed for ${characteristicUUID}:`, error);
      throw error;
    }
  }

  // Monitor characteristic for notifications
  async monitorCharacteristic(
    characteristicUUID: string,
    callback: (data: number[]) => void
  ): Promise<() => void> {
    if (!this.connectedDevice) {
      throw new Error('No device connected');
    }

    // Store device ID locally to avoid null reference issues during cleanup
    const deviceId = this.connectedDevice.id;

    try {
      // Start monitoring
      BleManager.startNotification(
        deviceId,
        BLE_UUIDS.SERVICE,
        characteristicUUID
      );

      // Set up notification listener
      const notificationListener = this.eventEmitter.addListener('BleManagerDidUpdateValueForCharacteristic', (data) => {
        if (data.peripheral === deviceId && 
            data.characteristic === characteristicUUID) {
          try {
            let bytes: number[];
            
            // Handle different data formats from react-native-ble-manager
                         if (Array.isArray(data.value)) {
               // Data is already a byte array - this is what we get from ESP32
               bytes = data.value;
               // Status data received
             } else if (typeof data.value === 'string') {
              // Data is a string, try to convert from base64
              try {
                bytes = this.base64ToArray(data.value);
              } catch (base64Error) {
                console.warn('Failed to convert from base64, treating as raw data:', base64Error);
                // If base64 conversion fails, treat as raw string
                bytes = Array.from(data.value as string).map((char: string) => char.charCodeAt(0));
              }
            } else {
              console.warn('Unknown data type from notification:', typeof data.value, data.value);
              bytes = [];
            }
            
            if (bytes.length > 0) {
              callback(bytes);
            }
          } catch (error) {
            console.error('Error processing notification data:', error);
          }
        }
      });

      // Return unsubscribe function
      return () => {
        try {
          // Use stored deviceId instead of this.connectedDevice which might be null
          BleManager.stopNotification(
            deviceId,
            BLE_UUIDS.SERVICE,
            characteristicUUID
          );
          notificationListener.remove();
        } catch (error) {
          console.error('Error stopping notification:', error);
        }
      };
    } catch (error) {
      console.error('Monitor characteristic failed:', error);
      throw error;
    }
  }

  // Start monitoring device status to keep connection alive
  async startStatusMonitoring(callback: (status: any) => void): Promise<() => void> {
    if (!this.connectedDevice) {
      throw new Error('No device connected');
    }

    // Clean up any existing status monitoring first
    if (this.statusUnsubscribe) {
      this.statusUnsubscribe();
      this.statusUnsubscribe = null;
    }

    try {
      // Monitor the status characteristic
      this.statusUnsubscribe = await this.monitorCharacteristic(
        BLE_UUIDS.STATUS,
        (data) => {
          // Parse status data and call callback
          try {
            // Parse ESP32 status data format
            // ESP32 sends JSON status as string: {"thr": 0.0, "mode": 1}
            try {
              // Convert byte array to string
              const statusString = String.fromCharCode(...data);
              
              // Parse JSON
              const statusJson = JSON.parse(statusString);
              
              if (statusJson.thr !== undefined && statusJson.mode !== undefined) {
                const status = {
                  timestamp: Date.now(),
                  throttle: statusJson.thr, // ESP32 sends throttle as 0.0-1.0
                  mode: statusJson.mode,   // ESP32 sends mode as 0, 1, or 2
                };
                
                // Track status changes
                this.lastStatusMode = status.mode;
                this.lastStatusThrottle = status.throttle;
                
                callback(status);
              } else {
                console.warn('Invalid status JSON format:', statusJson);
              }
            } catch (jsonError) {
              const statusString = String.fromCharCode(...data);
              console.warn('Failed to parse status JSON:', jsonError, 'Raw string:', statusString);
              
              // Only try to fix if it's clearly truncated
              if (statusString.includes('"thr"') && statusString.includes('"mode"') && !statusString.endsWith('}')) {
                try {
                  // Simple fix: add closing brace
                  const fixedString = statusString + '}';
                  const fixedJson = JSON.parse(fixedString);
                  
                  if (fixedJson.thr !== undefined && fixedJson.mode !== undefined) {
                    const status = {
                      timestamp: Date.now(),
                      throttle: fixedJson.thr,
                      mode: fixedJson.mode,
                    };
                    callback(status);
                    return;
                  }
                } catch (fixError) {
                  // Ignore fix errors, just log the original error
                }
              }
            }
          } catch (error) {
            console.error('Error parsing status data:', error);
            callback({
              timestamp: Date.now(),
              error: String(error),
              data: data
            });
          }
        }
      );
      
      return this.statusUnsubscribe;
    } catch (error) {
      console.error('Failed to start status monitoring:', error);
      throw error;
    }
  }

  // Keep connection alive by sending periodic pings
  startConnectionKeepAlive(): void {
    if (!this.connectedDevice) {
      return;
    }

    // Send a ping every 30 seconds to keep the connection alive
    const keepAliveInterval = setInterval(() => {
      if (this.connectedDevice) {
        try {
          // Try to read a simple characteristic to keep connection alive
          this.readCharacteristic(BLE_UUIDS.MODE).catch((error) => {
            console.warn('Keep-alive read failed:', error);
          });
        } catch (error) {
          console.warn('Keep-alive failed:', error);
        }
      } else {
        // Device disconnected, stop keep-alive
        clearInterval(keepAliveInterval);
      }
    }, 30000); // 30 seconds

    // Store the interval ID for cleanup
    this.keepAliveInterval = keepAliveInterval;
  }

  // Stop connection keep-alive
  stopConnectionKeepAlive(): void {
    if (this.keepAliveInterval) {
      clearInterval(this.keepAliveInterval);
      this.keepAliveInterval = null;
    }
  }

  // Disconnect from device
  async disconnect(): Promise<void> {
    if (this.connectedDevice) {
      try {
        const deviceId = this.connectedDevice.id;
        // Stop keep-alive before disconnecting
        this.stopConnectionKeepAlive();
        
        // Clean up status monitoring
        if (this.statusUnsubscribe) {
          this.statusUnsubscribe();
          this.statusUnsubscribe = null;
        }
        
        // Clear the connected device reference first to prevent race conditions
        this.connectedDevice = null;
        
        // Disconnect from BLE
        BleManager.disconnect(deviceId);
        
      } catch (error) {
        console.error('Disconnect error:', error);
        // Even if disconnect fails, clear the reference
        this.connectedDevice = null;
      }
    }
  }

  // Check if connected
  isConnected(): boolean {
    return this.connectedDevice !== null;
  }

  // Check connection health and reconnect if needed
  async checkConnectionHealth(): Promise<boolean> {
    if (!this.connectedDevice) {
      return false;
    }

    try {
      // Try to read a simple characteristic to test connection
      await this.readCharacteristic(BLE_UUIDS.MODE);
      return true;
    } catch (error) {
      console.warn('Connection health check failed:', error);
              // Connection might be lost, try to reconnect
        try {
          const device = this.connectedDevice;
          this.connectedDevice = null;
          
          // Reconnect
          await this.connectToDevice(device);
          await this.discoverServices(device.id);
          
          // Restart monitoring and keep-alive
          this.startConnectionKeepAlive();
          
          return true;
        } catch (reconnectError) {
        console.error('Reconnection failed:', reconnectError);
        return false;
      }
    }
  }



  // Get connected device
  getConnectedDevice(): BLEDevice | null {
    return this.connectedDevice;
  }

  // Get current BLE state
  async getState(): Promise<string> {
    return new Promise((resolve) => {
      // react-native-ble-manager doesn't have getState, we need to check differently
      // For now, return a default state and let the app handle it
      resolve('PoweredOn');
    });
  }

  // Check BLE permissions and status
  async checkBLEStatus(): Promise<{ hasPermissions: boolean; bluetoothEnabled: boolean; error?: string }> {
    try {
      // Try to start BLE manager to check if it's working
      try {
        BleManager.start({ showAlert: false });
        
        // Try a simple scan to test if scanning works
        return new Promise((resolve) => {
          const testTimeout = setTimeout(() => {
            resolve({ hasPermissions: true, bluetoothEnabled: true });
          }, 2000);
          
          try {
            // Try to start a scan - this will fail if we don't have permissions
            BleManager.scan([], 2, false);
            
            setTimeout(() => {
              try {
                BleManager.stopScan();
              } catch (stopError) {
                console.warn('Error stopping test scan:', stopError);
              }
              clearTimeout(testTimeout);
              resolve({ hasPermissions: true, bluetoothEnabled: true });
            }, 2000);
          } catch (scanError) {
            console.error('Test scan failed:', scanError);
            clearTimeout(testTimeout);
            
            // Check if it's a permission error
            const errorStr = String(scanError);
            if (errorStr.includes('permission') || errorStr.includes('denied') || errorStr.includes('unauthorized')) {
              resolve({ 
                hasPermissions: false, 
                bluetoothEnabled: true, 
                error: 'Permission denied - Location and Bluetooth permissions required' 
              });
            } else {
              resolve({ 
                hasPermissions: true, 
                bluetoothEnabled: false, 
                error: String(scanError) 
              });
            }
          }
        });
      } catch (startError) {
        console.error('Failed to start BLE manager:', startError);
        return { 
          hasPermissions: false, 
          bluetoothEnabled: false, 
          error: String(startError) 
        };
      }
    } catch (error) {
      console.error('BLE status check failed:', error);
      return { 
        hasPermissions: false, 
        bluetoothEnabled: false, 
        error: String(error) 
      };
    }
  }

  // Debug method: Scan for all devices
  async scanForAllDevices(): Promise<any[]> {
    if (this.isScanning) {
      return [];
    }

    try {
      this.isScanning = true;

      // Check if we're initialized
      if (!this.isInitialized) {
        await this.initialize();
      }

      return new Promise((resolve, reject) => {

        const allDevices: any[] = [];

        // Set up discovery listener
        const discoveryListener = this.eventEmitter.addListener('BleManagerDiscoverPeripheral', (data) => {
          allDevices.push(data);
        });

        // Set up scan stop listener
        const scanStopListener = this.eventEmitter.addListener('BleManagerStopScan', () => {
        });

        // Start scanning with more aggressive parameters
        try {
          // Try different scan parameters
          BleManager.scan([], 10, true);
        } catch (scanError) {
          // Try alternative parameters
          try {
            BleManager.scan([], 5, false);
          } catch (altError) {
            console.error('Failed to start scan with alternative parameters:', altError);
            this.isScanning = false;
            discoveryListener.remove();
            scanStopListener.remove();
            reject(new Error('Failed to start BLE scan'));
            return;
          }
        }
        
        // Return devices after timeout
        setTimeout(() => {
          this.isScanning = false;
          try {
            BleManager.stopScan();
          } catch (stopError) {
            console.warn('Error stopping scan:', stopError);
          }
          discoveryListener.remove();
          scanStopListener.remove();
          resolve(allDevices);
        }, 10000);
      });
    } catch (error) {
      this.isScanning = false;
      console.error('Scan failed:', error);
      throw error;
    }
  }

  // Convert number array to base64
  private arrayToBase64(array: number[]): string {
    const bytes = new Uint8Array(array);
    return btoa(String.fromCharCode(...bytes));
  }

  // Convert base64 to number array
  private base64ToArray(base64: string): number[] {
    try {
      const binaryString = atob(base64);
      const bytes = new Uint8Array(binaryString.length);
      for (let i = 0; i < binaryString.length; i++) {
        bytes[i] = binaryString.charCodeAt(i);
      }
      return Array.from(bytes);
    } catch (error) {
      console.error('Failed to convert base64 to array:', error);
      return [];
    }
  }

      // Cleanup event listeners
    cleanup(): void {
      this.eventListeners.forEach((listener) => {
        listener.remove();
      });
      this.eventListeners = [];
    }
}

// Create singleton instance
const bleManager = new AfterburnerBLEManager();

// Export the singleton instance
export default bleManager;

// Export the class for testing
export { AfterburnerBLEManager };
