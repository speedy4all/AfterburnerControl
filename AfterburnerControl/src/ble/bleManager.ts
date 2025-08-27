import { BleManager, Device, State } from 'react-native-ble-plx';
import { BLE_UUIDS, DEVICE_NAME } from './uuids';

class AfterburnerBLEManager {
  private manager: BleManager;
  private device: Device | null = null;
  private isScanning = false;

  constructor() {
    this.manager = new BleManager();
  }

  // Initialize BLE manager
  async initialize(): Promise<boolean> {
    try {
      const state = await this.manager.state();
      console.log('BLE State:', state);
      
      if (state === State.PoweredOn) {
        return true;
      }
      
      // Wait for BLE to be ready
      return new Promise((resolve) => {
        const subscription = this.manager.onStateChange((state) => {
          console.log('BLE State changed:', state);
          if (state === State.PoweredOn) {
            subscription.remove();
            resolve(true);
          }
        }, true);
      });
    } catch (error) {
      console.error('Failed to initialize BLE:', error);
      return false;
    }
  }

  // Scan for Afterburner device
  async scanForDevice(): Promise<Device | null> {
    if (this.isScanning) {
      console.log('Already scanning...');
      return null;
    }

    try {
      this.isScanning = true;
      console.log('Starting scan for device:', DEVICE_NAME);

      return new Promise((resolve, reject) => {
        const timeout = setTimeout(() => {
          this.isScanning = false;
          this.manager.stopDeviceScan();
          reject(new Error('Scan timeout'));
        }, 10000); // 10 second timeout

        this.manager.startDeviceScan(null, null, (error, device) => {
          if (error) {
            console.error('Scan error:', error);
            clearTimeout(timeout);
            this.isScanning = false;
            reject(error);
            return;
          }

          if (device && device.name === DEVICE_NAME) {
            console.log('Found Afterburner device:', device.name);
            clearTimeout(timeout);
            this.isScanning = false;
            this.manager.stopDeviceScan();
            resolve(device);
          }
        });
      });
    } catch (error) {
      this.isScanning = false;
      console.error('Scan failed:', error);
      throw error;
    }
  }

  // Connect to device
  async connectToDevice(device: Device): Promise<Device> {
    try {
      console.log('Connecting to device:', device.name);
      const connectedDevice = await device.connect();
      console.log('Connected to device');
      
      // Discover services
      await connectedDevice.discoverAllServicesAndCharacteristics();
      console.log('Services discovered');
      
      this.device = connectedDevice;
      return connectedDevice;
    } catch (error) {
      console.error('Connection failed:', error);
      throw error;
    }
  }

  // Disconnect from device
  async disconnect(): Promise<void> {
    if (this.device) {
      try {
        await this.device.cancelConnection();
        console.log('Disconnected from device');
      } catch (error) {
        console.error('Disconnect error:', error);
      }
      this.device = null;
    }
  }

  // Write data to characteristic
  async writeCharacteristic(
    characteristicUUID: string,
    data: number[]
  ): Promise<void> {
    if (!this.device) {
      throw new Error('No device connected');
    }

    try {
      const service = await this.device.services();
      const afterburnerService = service.find(s => s.uuid === BLE_UUIDS.SERVICE);
      
      if (!afterburnerService) {
        throw new Error('Afterburner service not found');
      }

      const characteristics = await afterburnerService.characteristics();
      const characteristic = characteristics.find(c => c.uuid === characteristicUUID);
      
      if (!characteristic) {
        throw new Error(`Characteristic ${characteristicUUID} not found`);
      }

      // Convert number array to base64 for BLE write
      const base64Data = Buffer.from(data).toString('base64');
      await characteristic.writeWithResponse(base64Data);
      console.log(`Wrote to ${characteristicUUID}:`, data);
    } catch (error) {
      console.error(`Write to ${characteristicUUID} failed:`, error);
      throw error;
    }
  }

  // Read characteristic
  async readCharacteristic(characteristicUUID: string): Promise<number[]> {
    if (!this.device) {
      throw new Error('No device connected');
    }

    try {
      const service = await this.device.services();
      const afterburnerService = service.find(s => s.uuid === BLE_UUIDS.SERVICE);
      
      if (!afterburnerService) {
        throw new Error('Afterburner service not found');
      }

      const characteristics = await afterburnerService.characteristics();
      const characteristic = characteristics.find(c => c.uuid === characteristicUUID);
      
      if (!characteristic) {
        throw new Error(`Characteristic ${characteristicUUID} not found`);
      }

      const characteristicValue = await characteristic.read();
      const data = Buffer.from(characteristicValue?.value || '', 'base64');
      return Array.from(data);
    } catch (error) {
      console.error(`Read from ${characteristicUUID} failed:`, error);
      throw error;
    }
  }

  // Monitor characteristic for notifications
  async monitorCharacteristic(
    characteristicUUID: string,
    callback: (data: number[]) => void
  ): Promise<() => void> {
    if (!this.device) {
      throw new Error('No device connected');
    }

    try {
      const service = await this.device.services();
      const afterburnerService = service.find(s => s.uuid === BLE_UUIDS.SERVICE);
      
      if (!afterburnerService) {
        throw new Error('Afterburner service not found');
      }

      const characteristics = await afterburnerService.characteristics();
      const characteristic = characteristics.find(c => c.uuid === characteristicUUID);
      
      if (!characteristic) {
        throw new Error(`Characteristic ${characteristicUUID} not found`);
      }

      const subscription = characteristic.monitor((error, characteristic) => {
        if (error) {
          console.error(`Monitor error for ${characteristicUUID}:`, error);
          return;
        }

        if (characteristic && characteristic.value) {
          const data = Buffer.from(characteristic.value, 'base64');
          callback(Array.from(data));
        }
      });

      return () => {
        subscription.remove();
      };
    } catch (error) {
      console.error(`Monitor setup for ${characteristicUUID} failed:`, error);
      throw error;
    }
  }

  // Get connected device
  getConnectedDevice(): Device | null {
    return this.device;
  }

  // Check if connected
  isConnected(): boolean {
    return this.device !== null;
  }

  // Destroy manager
  destroy(): void {
    this.manager.destroy();
  }
}

// Export singleton instance
export const bleManager = new AfterburnerBLEManager();
