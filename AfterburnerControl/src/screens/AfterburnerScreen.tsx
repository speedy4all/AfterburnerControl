import React, { useState, useEffect, useCallback } from 'react';
import {
  View,
  Text,
  StyleSheet,
  ScrollView,
  TouchableOpacity,
  Alert,
  TextInput,
  PermissionsAndroid,
  Platform,
} from 'react-native';
import AsyncStorage from '@react-native-async-storage/async-storage';
import { ColorInput } from '../components/ColorInput';
import {
  connectToAfterburner,
  disconnectFromAfterburner,
  writeMode,
  writeStartColor,
  writeEndColor,
  writeSpeedMs,
  writeBrightness,
  writeNumLeds,
  writeAbThreshold,
  savePreset,
  pushAllSettings,
  readSettings,
  setStatusCallback,
  clearStatusCallback,
  setCalibrationStatusCallback,
  clearCalibrationStatusCallback,
  startThrottleCalibration,
  readThrottleCalibrationStatus,
  resetThrottleCalibration,
  AfterburnerSettings,
  DeviceStatus,
} from '../ble/device';

// Throttle calibration functions now use the real BLE functions from device.ts
import { MODES, DEFAULT_VALUES } from '../ble/uuids';
import bleManager from '../ble/bleManager';

export const AfterburnerScreen: React.FC = () => {
  const [connected, setConnected] = useState(false);
  const [connecting, setConnecting] = useState(false);
  const [settings, setSettings] = useState<AfterburnerSettings>(DEFAULT_VALUES);
  const [status, setStatus] = useState<DeviceStatus | null>(null);
  const [bleState, setBleState] = useState<string>('Ready');
  const [bleAvailable, setBleAvailable] = useState<boolean>(true);
  const [_permissionsGranted, setPermissionsGranted] = useState<boolean>(false);
  const [_focusedInput, setFocusedInput] = useState<string | null>(null);
  const [isDarkTheme, setIsDarkTheme] = useState(false);
  
  // Theme persistence functions
  const saveThemePreference = async (isDark: boolean) => {
    try {
      await AsyncStorage.setItem('theme_preference', isDark ? 'dark' : 'light');
      console.log('🎨 Theme preference saved:', isDark ? 'dark' : 'light');
    } catch (error) {
      console.error('Failed to save theme preference:', error);
    }
  };

  const loadThemePreference = async () => {
    try {
      const savedTheme = await AsyncStorage.getItem('theme_preference');
      if (savedTheme) {
        const isDark = savedTheme === 'dark';
        setIsDarkTheme(isDark);
        console.log('🎨 Theme preference loaded:', isDark ? 'dark' : 'light');
      }
    } catch (error) {
      console.error('Failed to load theme preference:', error);
    }
  };

  const toggleTheme = () => {
    const newTheme = !isDarkTheme;
    setIsDarkTheme(newTheme);
    saveThemePreference(newTheme);
  };

  const resetThemeToDefault = async () => {
    try {
      await AsyncStorage.removeItem('theme_preference');
      setIsDarkTheme(false);
      console.log('🎨 Theme reset to default (light)');
    } catch (error) {
      console.error('Failed to reset theme preference:', error);
    }
  };
  
  // Local input values - separate from settings, only update ESP when send is clicked
  const [inputValues, setInputValues] = useState({
    speedMs: DEFAULT_VALUES.speedMs.toString(),
    brightness: DEFAULT_VALUES.brightness.toString(),
    numLeds: DEFAULT_VALUES.numLeds.toString(),
    abThreshold: DEFAULT_VALUES.abThreshold.toString(),
  });

  // Throttle calibration state
  const [throttleCalibration, setThrottleCalibration] = useState({
    isCalibrating: false,
    isCalibrated: false,
    min: 900,
    max: 2000,
  });

  // Request BLE permissions
  const requestBLEPermissions = async (): Promise<boolean> => {
    if (Platform.OS !== 'android') {
      return true; // iOS handles permissions differently
    }

    try {
      const permissions = [
        PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
        PermissionsAndroid.PERMISSIONS.ACCESS_COARSE_LOCATION,
        PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
        PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
      ];

      const results = await PermissionsAndroid.requestMultiple(permissions);
      
      const allGranted = Object.values(results).every(
        result => result === PermissionsAndroid.RESULTS.GRANTED
      );
      
      setPermissionsGranted(allGranted);
      
      if (!allGranted) {
        Alert.alert(
          'Permissions Required',
          'Bluetooth and Location permissions are required for BLE scanning to work. Please grant these permissions in Settings.',
          [
            { text: 'Cancel', style: 'cancel' },
            { text: 'Open Settings', onPress: () => {
              // This would ideally open app settings, but we'll just show the alert
            }}
          ]
        );
      }
      
      return allGranted;
    } catch (error) {
      console.error('Failed to request permissions:', error);
      return false;
    }
  };

  // Check BLE status on component mount
  useEffect(() => {
    const checkBLEStatus = async () => {
      try {
        // Check permissions first
        const hasPermissions = await requestBLEPermissions();
        if (!hasPermissions) {
          setBleState('Permission Denied');
          setBleAvailable(false);
          return;
        }
        
        // Since react-native-ble-manager doesn't provide state checking,
        // we'll assume BLE is available and let the connection process handle errors
        setBleState('Ready');
        setBleAvailable(true);
      } catch (error) {
        console.error('Failed to check BLE status:', error);
        setBleState('Error');
        setBleAvailable(false);
      }
    };
    
    checkBLEStatus();
    
    // Set up BLE state monitoring
    const checkInterval = setInterval(checkBLEStatus, 2000); // Check every 2 seconds
    
    return () => clearInterval(checkInterval);
  }, []);

  // Load theme preference on component mount
  useEffect(() => {
    loadThemePreference();
  }, []);

  // Connect to device
  const handleConnect = async () => {
    if (connected) {
      try {
        await disconnectFromAfterburner();
        setConnected(false);
      } catch (error) {
        Alert.alert('Error', 'Failed to disconnect');
      }
      return;
    }

    setConnecting(true);
    try {
      // BLE availability will be checked during the connection process

      const success = await connectToAfterburner();
      if (success) {
        setConnected(true);
        
        // Read current settings only once on initial connection
        try {
          const currentSettings = await readSettings();
          setSettings(currentSettings);
        } catch (error) {
          // Keep the current local settings if reading fails
        }
        
        // Read initial calibration status
        try {
          const calibrationStatus = await readThrottleCalibrationStatus();
          setThrottleCalibration(prev => ({
            ...prev,
            isCalibrated: calibrationStatus.isCalibrated,
            min: calibrationStatus.min,
            max: calibrationStatus.max,
          }));
        } catch (error) {
          console.warn('Failed to read initial calibration status:', error);
          // Continue anyway - status will be updated via BLE notifications
        }
        
        // Status monitoring is now handled automatically by the BLE manager
        // The status will be updated through the connection health check
        
        Alert.alert('Success', 'Connected to Afterburner device');
      } else {
        Alert.alert('Error', 'Failed to connect to device');
      }
    } catch (error) {
      console.error('Connection error:', error);
      let errorMessage = 'Failed to connect to device';
      
      if (error instanceof Error) {
        if (error.message.includes('Bluetooth is disabled')) {
          errorMessage = 'Bluetooth is disabled. Please enable Bluetooth in device settings.';
        } else if (error.message.includes('permission denied')) {
          errorMessage = 'Bluetooth permission denied. Please grant Bluetooth permissions in device settings.';
        } else if (error.message.includes('not supported')) {
          errorMessage = 'Bluetooth Low Energy is not supported on this device.';
        } else if (error.message.includes('timeout')) {
          errorMessage = 'Connection timeout. Please ensure the device is nearby and try again.';
        } else {
          errorMessage = error.message;
        }
      }
      
      Alert.alert('Connection Error', errorMessage);
    } finally {
      setConnecting(false);
    }
  };

  // Update settings
  const updateSettings = useCallback((newSettings: Partial<AfterburnerSettings>) => {
    setSettings(prev => ({ ...prev, ...newSettings }));
  }, []);

  // Send individual settings
  const sendMode = async () => {
    try {
      await writeMode(settings.mode);
      Alert.alert('Success', 'Mode updated');
    } catch (error) {
      Alert.alert('Error', 'Failed to update mode');
    }
  };

  const sendStartColor = async () => {
    try {
      await writeStartColor(settings.startColor);
      Alert.alert('Success', 'Start color updated');
    } catch (error) {
      Alert.alert('Error', 'Failed to update start color');
    }
  };

  const sendEndColor = async () => {
    try {
      await writeEndColor(settings.endColor);
      Alert.alert('Success', 'End color updated');
    } catch (error) {
      Alert.alert('Error', 'Failed to update end color');
    }
  };

  const sendSpeed = async () => {
    try {
      const speedValue = parseInt(inputValues.speedMs);
      if (isNaN(speedValue) || speedValue < 100 || speedValue > 5000) {
        Alert.alert('Invalid Input', 'Speed must be between 100-5000 ms');
        return;
      }
      await writeSpeedMs(speedValue);
      // Update local settings after successful send
      updateSettings({ speedMs: speedValue });
      Alert.alert('Success', 'Speed updated');
    } catch (error) {
      Alert.alert('Error', 'Failed to update speed');
    }
  };

  const sendBrightness = async () => {
    try {
      const brightnessValue = parseInt(inputValues.brightness);
      if (isNaN(brightnessValue) || brightnessValue < 10 || brightnessValue > 255) {
        Alert.alert('Invalid Input', 'Brightness must be between 10-255');
        return;
      }
      await writeBrightness(brightnessValue);
      // Update local settings after successful send
      updateSettings({ brightness: brightnessValue });
      Alert.alert('Success', 'Brightness updated');
    } catch (error) {
      Alert.alert('Error', 'Failed to update brightness');
    }
  };

  const sendNumLeds = async () => {
    try {
      const numLedsValue = parseInt(inputValues.numLeds);
      if (isNaN(numLedsValue) || numLedsValue < 1 || numLedsValue > 100) {
        Alert.alert('Invalid Input', 'Number of LEDs must be between 1-100');
        return;
      }
      await writeNumLeds(numLedsValue);
      // Update local settings after successful send
      updateSettings({ numLeds: numLedsValue });
      Alert.alert('Success', 'Number of LEDs updated');
    } catch (error) {
      Alert.alert('Error', 'Failed to update number of LEDs');
    }
  };

  const sendAbThreshold = async () => {
    try {
      const thresholdValue = parseInt(inputValues.abThreshold);
      if (isNaN(thresholdValue) || thresholdValue < 0 || thresholdValue > 100) {
        Alert.alert('Invalid Input', 'Afterburner threshold must be between 0-100%');
        return;
      }
      await writeAbThreshold(thresholdValue);
      // Update local settings after successful send
      updateSettings({ abThreshold: thresholdValue });
      Alert.alert('Success', 'Afterburner threshold updated');
    } catch (error) {
      Alert.alert('Error', 'Failed to update afterburner threshold');
    }
  };

  // Throttle calibration functions
  const handleStartThrottleCalibration = async () => {
    try {
      setThrottleCalibration(prev => ({ ...prev, isCalibrating: true }));
      
      // Trigger ESP to start reading receiver values
      await startThrottleCalibration();
      
      // No popup needed - the UI will automatically update via BLE notifications
      // The ESP will send status updates as calibration progresses
    } catch (error) {
      setThrottleCalibration(prev => ({ ...prev, isCalibrating: false }));
      console.error('Failed to start calibration:', error);
      // UI will show error state automatically
    }
  };

  const checkCalibrationStatus = async () => {
    try {
      // Read current calibration values from ESP (manual check if needed)
      const calibrationData = await readThrottleCalibrationStatus();
      
      setThrottleCalibration(prev => ({
        ...prev,
        isCalibrated: calibrationData.isCalibrated,
        min: calibrationData.min,
        max: calibrationData.max,
        isCalibrating: false,
      }));
      
      // No popup needed - the UI will show the current status
    } catch (error) {
      console.error('Failed to check calibration status:', error);
      // UI will show error state automatically
    }
  };

  const handleResetThrottleCalibration = async () => {
    try {
      // Send reset command to ESP first
      await resetThrottleCalibration();
      
      // Reset to default values
      const defaultCalibration = { min: 900, max: 2000 };
      setThrottleCalibration(prev => ({
        ...prev,
        min: defaultCalibration.min,
        max: defaultCalibration.max,
        isCalibrated: false,
        isCalibrating: false,
      }));
      
      // No popup needed - the UI will show the reset status
    } catch (error) {
      console.error('Failed to reset throttle calibration:', error);
      // UI will show error state automatically
    }
  };

  // Push all settings
  const handlePushAll = async () => {
    try {
      await pushAllSettings(settings);
      Alert.alert('Success', 'All settings pushed to device');
    } catch (error) {
      Alert.alert('Error', 'Failed to push settings');
    }
  };

  // Save preset
  const handleSavePreset = async () => {
    try {
      await savePreset();
      Alert.alert('Success', 'Preset saved to device');
    } catch (error) {
      Alert.alert('Error', 'Failed to save preset');
    }
  };

  // Set up status callback when connected
  useEffect(() => {
    if (connected) {
      // Set up status callback to receive updates from BLE manager
      setStatusCallback((newStatus) => {
        setStatus(newStatus);
      });
      
      // Set up calibration status callback to receive updates from BLE manager
      setCalibrationStatusCallback((calibrationStatus) => {
        console.log('🎯 App received calibration status update:', calibrationStatus);
        setThrottleCalibration(prev => {
          const newState = {
            ...prev,
            isCalibrated: calibrationStatus.isCalibrated,
            min: calibrationStatus.min,
            max: calibrationStatus.max,
            // If calibration is complete, stop the calibrating state
            isCalibrating: !calibrationStatus.isCalibrated && (calibrationStatus.min > 0 || calibrationStatus.max > 0)
          };
          console.log('🎯 Updated throttle calibration state:', newState);
          return newState;
        });
        
        // Log progress if available
        if (calibrationStatus.minVisits !== undefined && calibrationStatus.maxVisits !== undefined) {
          console.log(`🎯 Calibration progress - Min visits: ${calibrationStatus.minVisits}/3, Max visits: ${calibrationStatus.maxVisits}/3`);
        }
      });
      
      // Clean up callbacks when disconnecting
      return () => {
        clearStatusCallback();
        clearCalibrationStatusCallback();
      };
    }
  }, [connected]);

  // Connection health check
  useEffect(() => {
    if (connected) {
      const healthCheckInterval = setInterval(async () => {
        try {
          const isHealthy = await bleManager.checkConnectionHealth();
          if (!isHealthy) {
            setConnected(false);
            setStatus({ throttle: 0, mode: 0 });
          }
        } catch (error) {
          console.error('Health check error:', error);
        }
      }, 10000); // Check every 10 seconds

      return () => clearInterval(healthCheckInterval);
    }
  }, [connected]);

  // Cleanup on unmount
  useEffect(() => {
    return () => {
      if (connected) {
        disconnectFromAfterburner();
      }
    };
  }, [connected]);

  // Sync input values when settings are loaded from device
  useEffect(() => {
    setInputValues({
      speedMs: settings.speedMs.toString(),
      brightness: settings.brightness.toString(),
      numLeds: settings.numLeds.toString(),
      abThreshold: settings.abThreshold.toString(),
    });
  }, [settings.speedMs, settings.brightness, settings.numLeds, settings.abThreshold]);
 
  return (
         <ScrollView style={[styles.container, isDarkTheme && styles.containerDark]}>
             <View style={[styles.header, isDarkTheme && styles.headerDark]}>
                   <View style={styles.headerTop}>
            <Text style={[styles.title, isDarkTheme && styles.titleDark]}>Afterburner Control</Text>
            {/* Theme toggle: tap to switch, long press to reset to default */}
            <TouchableOpacity
              style={[styles.themeToggle, isDarkTheme && styles.themeToggleDark]}
              onPress={toggleTheme}
              onLongPress={resetThemeToDefault}
            >
              <Text style={styles.themeToggleText}>{isDarkTheme ? '☀️' : '🌙'}</Text>
            </TouchableOpacity>
          </View>
        
                 {/* BLE Status Indicator */}
         <View style={styles.bleStatusContainer}>
           <View style={styles.bleStatusRow}>
             <Text style={[styles.bleStatusLabel, isDarkTheme && styles.bleStatusLabelDark]}>Bluetooth Status:</Text>
            <TouchableOpacity 
              style={styles.refreshButton} 
              onPress={async () => {
                try {
                  if (connected) {
                    // Check connection health
                    const isHealthy = await bleManager.checkConnectionHealth();
                    if (isHealthy) {
                      setBleState('Connected');
                      setBleAvailable(true);
                    } else {
                      setBleState('Disconnected');
                      setBleAvailable(false);
                      setConnected(false);
                    }
                  } else {
                    // Since we can't get actual BLE state, just refresh the display
                    setBleState('Ready');
                    setBleAvailable(true);
                  }
                } catch (error) {
                  console.error('Failed to refresh BLE state:', error);
                 }
               }}
            >
              <Text style={styles.refreshButtonText}>↻</Text>
            </TouchableOpacity>
          </View>
                     <View style={[styles.bleStatusIndicator, bleAvailable ? styles.bleStatusGood : styles.bleStatusBad]}>
             <Text style={[styles.bleStatusText, bleAvailable ? styles.bleStatusTextGood : styles.bleStatusTextBad]}>
               {bleState === 'Ready' ? 'Ready' : 
                bleState === 'Permission Denied' ? 'Permission Denied' :
                bleState === 'Error' ? 'Error' : 
                bleState}
             </Text>
           </View>
                     {!bleAvailable && (
             <Text style={styles.bleStatusHelp}>
               {bleState === 'Error' ? 'Check Bluetooth settings and restart the app' :
                'Bluetooth may not be available'}
             </Text>
           )}
        </View>
        
                 <TouchableOpacity
           style={[
             styles.connectButton,
             connected ? styles.disconnectButton : styles.connectButton,
             connecting && styles.connectingButton,
           ]}
           onPress={handleConnect}
           disabled={connecting || !bleAvailable}
         >
           <Text style={styles.connectButtonText}>
             {connecting ? 'Connecting...' : connected ? 'Disconnect' : 'Connect'}
           </Text>
         </TouchableOpacity>
         

         
                   
      </View>

      {/* Simulator Warning */}
      

      {connected && (
        <>
                     {/* Status Display */}
           <View style={[styles.statusContainer, isDarkTheme && styles.statusContainerDark]}>
             <Text style={[styles.sectionTitle, isDarkTheme && styles.sectionTitleDark]}>Device Status</Text>
                         {status ? (
               <View style={styles.statusInfo}>
                 <Text style={[styles.statusText, isDarkTheme && styles.statusTextDark]}>
                   Throttle: <Text style={[styles.statusValue, isDarkTheme && styles.statusValueDark]}>{(status.throttle * 100).toFixed(1)}%</Text>
                 </Text>
                 <Text style={[styles.statusText, isDarkTheme && styles.statusTextDark]}>
                   Mode: <Text style={[styles.statusValue, isDarkTheme && styles.statusValueDark]}>{MODES[status.mode]?.name || 'Unknown'}</Text>
                 </Text>
               </View>
             ) : (
               <Text style={[styles.statusText, isDarkTheme && styles.statusTextDark]}>
                 {connected ? 'Connected - Waiting for status updates...' : 'Disconnected'}
               </Text>
             )}
          </View>

                     {/* Mode Selection */}
           <View style={[styles.section, isDarkTheme && styles.sectionDark]}>
             <Text style={[styles.sectionTitle, isDarkTheme && styles.sectionTitleDark]}>Mode</Text>
            <View style={styles.modeButtons}>
              {MODES.map((mode) => (
                <TouchableOpacity
                  key={mode.id}
                  style={[
                    styles.modeButton,
                    settings.mode === mode.id && styles.modeButtonActive,
                  ]}
                  onPress={() => updateSettings({ mode: mode.id })}
                >
                  <Text
                    style={[
                      styles.modeButtonText,
                      settings.mode === mode.id && styles.modeButtonTextActive,
                    ]}
                  >
                    {mode.name}
                  </Text>
                </TouchableOpacity>
              ))}
            </View>
            <TouchableOpacity style={styles.sendButton} onPress={sendMode}>
              <Text style={styles.sendButtonText}>Send Mode</Text>
            </TouchableOpacity>
          </View>

                     {/* Color Controls */}
           <View style={[styles.section, isDarkTheme && styles.sectionDark]}>
             <Text style={[styles.sectionTitle, isDarkTheme && styles.sectionTitleDark]}>Colors</Text>
                         <ColorInput
               label="Start Color"
               color={settings.startColor}
               onColorChange={(color) => updateSettings({ startColor: color })}
               onSend={sendStartColor}
               isDarkTheme={isDarkTheme}
             />
             <ColorInput
               label="End Color"
               color={settings.endColor}
               onColorChange={(color) => updateSettings({ endColor: color })}
               onSend={sendEndColor}
               isDarkTheme={isDarkTheme}
             />
          </View>

                     {/* Speed Control */}
           <View style={[styles.section, isDarkTheme && styles.sectionDark]}>
             <Text style={[styles.sectionTitle, isDarkTheme && styles.sectionTitleDark]}>Speed (ms) - 100-5000</Text>
            <View style={styles.inputRow}>
                             <TextInput
                 style={[styles.numberInput, isDarkTheme && styles.numberInputDark]}
                 value={inputValues.speedMs}
                onChangeText={(text) => {
                  setInputValues(prev => ({ ...prev, speedMs: text }));
                }}
                keyboardType="numeric"
                placeholder="1200"
                editable={true}
                selectTextOnFocus={true}
                clearButtonMode="while-editing"
                returnKeyType="done"
                blurOnSubmit={true}
                onFocus={() => setFocusedInput('speedMs')}
                onBlur={() => setFocusedInput(null)}
              />
              <TouchableOpacity style={styles.sendButton} onPress={sendSpeed}>
                <Text style={styles.sendButtonText}>Send</Text>
              </TouchableOpacity>
            </View>
            <Text style={styles.rangeText}>Range: 100-5000 ms</Text>
          </View>

                     {/* Brightness Control */}
           <View style={[styles.section, isDarkTheme && styles.sectionDark]}>
             <Text style={[styles.sectionTitle, isDarkTheme && styles.sectionTitleDark]}>Brightness - 10-255</Text>
            <View style={styles.inputRow}>
                             <TextInput
                 style={[styles.numberInput, isDarkTheme && styles.numberInputDark]}
                 value={inputValues.brightness}
                onChangeText={(text) => {
                  setInputValues(prev => ({ ...prev, brightness: text }));
                }}
                keyboardType="numeric"
                placeholder="200"
                editable={true}
                selectTextOnFocus={true}
                clearButtonMode="while-editing"
                returnKeyType="done"
                blurOnSubmit={true}
                onFocus={() => setFocusedInput('brightness')}
                onBlur={() => setFocusedInput(null)}
              />
              <TouchableOpacity style={styles.sendButton} onPress={sendBrightness}>
                <Text style={styles.sendButtonText}>Send</Text>
              </TouchableOpacity>
            </View>
            <Text style={styles.rangeText}>Range: 10-255</Text>
          </View>

                     {/* Number of LEDs */}
           <View style={[styles.section, isDarkTheme && styles.sectionDark]}>
             <Text style={[styles.sectionTitle, isDarkTheme && styles.sectionTitleDark]}>Number of LEDs - 1-100</Text>
            <View style={styles.inputRow}>
                             <TextInput
                 style={[styles.numberInput, isDarkTheme && styles.numberInputDark]}
                 value={inputValues.numLeds}
                onChangeText={(text) => {
                  setInputValues(prev => ({ ...prev, numLeds: text }));
                }}
                keyboardType="numeric"
                placeholder="45"
                editable={true}
                selectTextOnFocus={true}
                clearButtonMode="while-editing"
                returnKeyType="done"
                blurOnSubmit={true}
                onFocus={() => setFocusedInput('numLeds')}
                onBlur={() => setFocusedInput(null)}
              />
              <TouchableOpacity style={styles.sendButton} onPress={sendNumLeds}>
                <Text style={styles.sendButtonText}>Send</Text>
              </TouchableOpacity>
            </View>
            <Text style={styles.rangeText}>Range: 1-100 LEDs</Text>
          </View>

                                {/* Afterburner Threshold */}
           <View style={[styles.section, isDarkTheme && styles.sectionDark]}>
             <Text style={[styles.sectionTitle, isDarkTheme && styles.sectionTitleDark]}>Afterburner Threshold (%) - 0-100</Text>
             <View style={styles.inputRow}>
               <TextInput
                 style={[styles.numberInput, isDarkTheme && styles.numberInputDark]}
                 value={inputValues.abThreshold}
                 onChangeText={(text) => {
                   setInputValues(prev => ({ ...prev, abThreshold: text }));
                 }}
                 keyboardType="numeric"
                 placeholder="80"
                 editable={true}
                 selectTextOnFocus={true}
                 clearButtonMode="while-editing"
                 returnKeyType="done"
                 blurOnSubmit={true}
                 onFocus={() => setFocusedInput('abThreshold')}
                 onBlur={() => setFocusedInput(null)}
               />
               <TouchableOpacity style={styles.sendButton} onPress={sendAbThreshold}>
                 <Text style={styles.sendButtonText}>Send</Text>
               </TouchableOpacity>
             </View>
             <Text style={styles.rangeText}>Range: 0-100%</Text>
           </View>

                       {/* Throttle Calibration */}
            <View style={[styles.section, isDarkTheme && styles.sectionDark]}>
              <Text style={[styles.sectionTitle, isDarkTheme && styles.sectionTitleDark]}>Throttle Calibration (PWM)</Text>
              
              {/* Calibration Status */}
              <View style={styles.calibrationStatus}>
                <View style={styles.statusRow}>
                  <Text style={[styles.statusLabel, isDarkTheme && styles.statusLabelDark]}>Status:</Text>
                  <View style={[
                    styles.statusIndicator,
                    throttleCalibration.isCalibrating ? styles.statusCalibrating :
                    throttleCalibration.isCalibrated ? styles.statusComplete : styles.statusPending
                  ]}>
                    <Text style={styles.statusIndicatorText}>
                      {throttleCalibration.isCalibrating ? 'Calibrating...' :
                       throttleCalibration.isCalibrated ? 'Complete' : 'Not Calibrated'}
                    </Text>
                  </View>
                </View>
                
                                 {/* Show current values during calibration */}
                 {(throttleCalibration.isCalibrating || throttleCalibration.isCalibrated) && (
                   <View style={[styles.calibrationValues, isDarkTheme && styles.calibrationValuesDark]}>
                     <Text style={[styles.calibrationValue, isDarkTheme && styles.calibrationValueDark]}>
                       Min: {throttleCalibration.min} PWM
                     </Text>
                     <Text style={[styles.calibrationValue, isDarkTheme && styles.calibrationValueDark]}>
                       Max: {throttleCalibration.max} PWM
                     </Text>
                     {throttleCalibration.isCalibrating && (
                       <Text style={[styles.calibrationProgress, isDarkTheme && styles.calibrationProgressDark]}>
                         Range: {throttleCalibration.max - throttleCalibration.min} PWM
                       </Text>
                     )}
                   </View>
                 )}
              </View>
              
              <Text style={[styles.calibrationInfo, isDarkTheme && styles.calibrationInfoDark]}>
                {throttleCalibration.isCalibrating 
                  ? 'Move throttle stick to min and max positions. ESP will read and save values automatically. Status updates in real-time.'
                  : 'Calibrate throttle input range by reading actual receiver values. Values are saved to ESP memory.'}
              </Text>
              
              {/* Calibration Buttons */}
              <View style={styles.calibrationButtons}>
                                 <TouchableOpacity 
                   style={[
                     styles.calibrationButton,
                     throttleCalibration.isCalibrating && styles.calibrationButtonDisabled
                   ]} 
                   onPress={handleStartThrottleCalibration}
                   disabled={throttleCalibration.isCalibrating}
                 >
                  <Text style={styles.calibrationButtonText}>
                    {throttleCalibration.isCalibrating ? 'Calibrating...' : 'Start Calibration'}
                  </Text>
                </TouchableOpacity>
                
                <TouchableOpacity 
                  style={styles.checkButton} 
                  onPress={checkCalibrationStatus}
                >
                  <Text style={styles.checkButtonText}>Check Status</Text>
                </TouchableOpacity>
              </View>
              
              <TouchableOpacity 
                style={styles.resetButton} 
                onPress={handleResetThrottleCalibration}
              >
                <Text style={styles.resetButtonText}>Reset to Defaults</Text>
              </TouchableOpacity>
            </View>

                     {/* Action Buttons */}
           <View style={[styles.section, isDarkTheme && styles.sectionDark]}>
             <Text style={[styles.sectionTitle, isDarkTheme && styles.sectionTitleDark]}>Actions</Text>
            <TouchableOpacity style={styles.actionButton} onPress={handlePushAll}>
              <Text style={styles.actionButtonText}>Push All Settings</Text>
            </TouchableOpacity>
            <TouchableOpacity style={styles.actionButton} onPress={handleSavePreset}>
              <Text style={styles.actionButtonText}>Save Preset</Text>
            </TouchableOpacity>
          </View>
        </>
      )}
    </ScrollView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#f8f9fa',
  },
  containerDark: {
    backgroundColor: '#1a1a1a',
  },
  header: {
    flexDirection: 'column',
    alignItems: 'center',
    padding: 16,
    backgroundColor: 'white',
    borderBottomWidth: 1,
    borderBottomColor: '#e0e0e0',
    gap: 12,
  },
  headerDark: {
    backgroundColor: '#2d2d2d',
    borderBottomColor: '#404040',
  },
  headerTop: {
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'space-between',
    width: '100%',
    marginBottom: 12,
  },
  title: {
    fontSize: 24,
    fontWeight: 'bold',
    color: '#333',
  },
  titleDark: {
    color: '#ffffff',
  },
  themeToggle: {
    padding: 8,
    borderRadius: 20,
    backgroundColor: '#f8f9fa',
    borderWidth: 1,
    borderColor: '#dee2e6',
  },
  themeToggleDark: {
    backgroundColor: '#404040',
    borderColor: '#666',
  },
  themeToggleText: {
    fontSize: 18,
    fontWeight: 'bold',
  },
  bleStatusContainer: {
    alignItems: 'center',
    marginBottom: 12,
  },
  bleStatusRow: {
    flexDirection: 'row',
    alignItems: 'center',
    gap: 8,
    marginBottom: 4,
  },
  button: {
    paddingHorizontal: 20,
    paddingVertical: 10,
    borderRadius: 8,
    alignItems: 'center',
    justifyContent: 'center',
    marginHorizontal: 8,
  },
  buttonText: {
    color: 'white',
    fontSize: 16,
    fontWeight: '600',
  },

  refreshButton: {
    padding: 4,
    borderRadius: 12,
    backgroundColor: '#f8f9fa',
    borderWidth: 1,
    borderColor: '#dee2e6',
  },
  refreshButtonText: {
    fontSize: 16,
    color: '#6c757d',
    fontWeight: 'bold',
  },
  bleStatusLabel: {
    fontSize: 14,
    color: '#666',
    marginBottom: 4,
  },
  bleStatusLabelDark: {
    color: '#cccccc',
  },
  bleStatusIndicator: {
    paddingHorizontal: 12,
    paddingVertical: 6,
    borderRadius: 16,
    minWidth: 80,
    alignItems: 'center',
  },
  bleStatusGood: {
    backgroundColor: '#d4edda',
    borderWidth: 1,
    borderColor: '#c3e6cb',
  },
  bleStatusBad: {
    backgroundColor: '#f8d7da',
    borderWidth: 1,
    borderColor: '#f5c6cb',
  },
  bleStatusText: {
    fontSize: 12,
    fontWeight: '600',
  },
  bleStatusTextGood: {
    color: '#155724',
  },
  bleStatusTextBad: {
    color: '#721c24',
  },
  bleStatusHelp: {
    fontSize: 12,
    color: '#721c24',
    textAlign: 'center',
    marginTop: 4,
    fontStyle: 'italic',
  },
  connectButton: {
    backgroundColor: '#28a745',
    paddingHorizontal: 20,
    paddingVertical: 10,
    borderRadius: 8,
  },
  disconnectButton: {
    backgroundColor: '#dc3545',
  },
  connectingButton: {
    backgroundColor: '#6c757d',
  },
  connectButtonText: {
    color: 'white',
    fontSize: 16,
    fontWeight: '600',
  },
  statusContainer: {
    margin: 16,
    padding: 16,
    backgroundColor: 'white',
    borderRadius: 8,
    borderWidth: 1,
    borderColor: '#e0e0e0',
  },
  statusContainerDark: {
    backgroundColor: '#2d2d2d',
    borderColor: '#404040',
  },
  section: {
    margin: 16,
    padding: 16,
    backgroundColor: 'white',
    borderRadius: 8,
    borderWidth: 1,
    borderColor: '#e0e0e0',
  },
  sectionDark: {
    backgroundColor: '#2d2d2d',
    borderColor: '#404040',
  },
  sectionTitle: {
    fontSize: 18,
    fontWeight: '600',
    color: '#333',
    marginBottom: 12,
  },
  sectionTitleDark: {
    color: '#ffffff',
  },
  statusInfo: {
    gap: 8,
  },
  statusText: {
    fontSize: 16,
    color: '#666',
  },
  statusTextDark: {
    color: '#ffffff',
  },
  statusValue: {
    fontWeight: 'bold',
    color: '#007AFF',
  },
  statusValueDark: {
    fontWeight: 'bold',
    color: '#4DA6FF',
  },
  modeButtons: {
    flexDirection: 'row',
    gap: 8,
    marginBottom: 12,
  },
  modeButton: {
    flex: 1,
    paddingVertical: 12,
    paddingHorizontal: 16,
    borderRadius: 8,
    borderWidth: 1,
    borderColor: '#007AFF',
    alignItems: 'center',
  },
  modeButtonActive: {
    backgroundColor: '#007AFF',
  },
  modeButtonText: {
    fontSize: 14,
    fontWeight: '600',
    color: '#007AFF',
  },
  modeButtonTextActive: {
    color: 'white',
  },
  inputRow: {
    flexDirection: 'row',
    alignItems: 'center',
    gap: 12,
  },
  numberInput: {
    flex: 1,
    height: 48,
    borderWidth: 2,
    borderColor: '#007AFF',
    borderRadius: 8,
    paddingHorizontal: 16,
    fontSize: 18,
    backgroundColor: 'white',
    color: '#333',
    textAlign: 'center',
    fontWeight: '500',
  },
  numberInputDark: {
    backgroundColor: '#404040',
    color: '#ffffff',
    borderColor: '#007AFF',
  },
  sendButton: {
    backgroundColor: '#007AFF',
    paddingHorizontal: 16,
    paddingVertical: 10,
    borderRadius: 8,
  },
  sendButtonText: {
    color: 'white',
    fontSize: 14,
    fontWeight: '600',
  },
  rangeText: {
    fontSize: 12,
    color: '#666',
    fontStyle: 'italic',
    marginTop: 4,
  },
  calibrationInfo: {
    fontSize: 14,
    color: '#666',
    fontStyle: 'italic',
    marginBottom: 16,
    textAlign: 'center',
  },
  calibrationInfoDark: {
    color: '#cccccc',
  },
  calibrationStatus: {
    marginBottom: 16,
  },
  statusRow: {
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'space-between',
    marginBottom: 12,
  },
  statusLabel: {
    fontSize: 16,
    fontWeight: '600',
    color: '#333',
  },
  statusLabelDark: {
    color: '#ffffff',
  },
  statusIndicator: {
    paddingHorizontal: 12,
    paddingVertical: 6,
    borderRadius: 16,
    minWidth: 100,
    alignItems: 'center',
  },
  statusPending: {
    backgroundColor: '#fff3cd',
    borderWidth: 1,
    borderColor: '#ffeaa7',
  },
  statusCalibrating: {
    backgroundColor: '#d1ecf1',
    borderWidth: 1,
    borderColor: '#bee5eb',
  },
  statusComplete: {
    backgroundColor: '#d4edda',
    borderWidth: 1,
    borderColor: '#c3e6cb',
  },
  statusIndicatorText: {
    fontSize: 12,
    fontWeight: '600',
    color: '#333',
  },
  calibrationValues: {
    backgroundColor: '#f8f9fa',
    padding: 12,
    borderRadius: 8,
    marginBottom: 12,
  },
  calibrationValuesDark: {
    backgroundColor: '#404040',
  },
  calibrationValue: {
    fontSize: 14,
    color: '#333',
    fontWeight: '500',
    marginBottom: 4,
  },
  calibrationValueDark: {
    color: '#ffffff',
  },
  calibrationProgress: {
    fontSize: 12,
    color: '#666',
    fontStyle: 'italic',
    marginTop: 4,
  },
  calibrationProgressDark: {
    color: '#cccccc',
  },
  inputLabel: {
    fontSize: 16,
    fontWeight: '600',
    color: '#333',
    minWidth: 50,
  },
  inputLabelDark: {
    color: '#ffffff',
  },
  calibrationButtons: {
    flexDirection: 'row',
    gap: 12,
    marginTop: 16,
    marginBottom: 12,
  },
  calibrationButton: {
    flex: 1,
    backgroundColor: '#28a745',
    paddingVertical: 12,
    paddingHorizontal: 16,
    borderRadius: 8,
    alignItems: 'center',
  },
  calibrationButtonDisabled: {
    backgroundColor: '#6c757d',
    opacity: 0.6,
  },
  calibrationButtonText: {
    color: 'white',
    fontSize: 14,
    fontWeight: '600',
  },
  checkButton: {
    flex: 1,
    backgroundColor: '#17a2b8',
    paddingVertical: 12,
    paddingHorizontal: 16,
    borderRadius: 8,
    alignItems: 'center',
  },
  checkButtonText: {
    color: 'white',
    fontSize: 14,
    fontWeight: '600',
  },
  resetButton: {
    backgroundColor: '#6c757d',
    paddingVertical: 12,
    paddingHorizontal: 16,
    borderRadius: 8,
    alignItems: 'center',
  },
  resetButtonText: {
    color: 'white',
    fontSize: 14,
    fontWeight: '600',
  },
  actionButton: {
    backgroundColor: '#17a2b8',
    paddingVertical: 12,
    paddingHorizontal: 20,
    borderRadius: 8,
    marginBottom: 8,
    alignItems: 'center',
  },
     actionButtonText: {
     color: 'white',
     fontSize: 16,
     fontWeight: '600',
   },
   

});
