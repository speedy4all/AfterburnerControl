import React, { useState, useEffect, useCallback } from 'react';
import {
  View,
  Text,
  StyleSheet,
  ScrollView,
  TouchableOpacity,
  Alert,
  TextInput,
} from 'react-native';
import { ColorInput } from '../components/ColorInput';
import {
  connectToAfterburner,
  disconnectFromAfterburner,
  savePreset,
  pushAllSettings,
  readSettings,
  monitorStatus,
  monitorSettings,
  startThrottleCalibration,
  resetThrottleCalibration,
  AfterburnerSettings,
  DeviceStatus,
} from '../websocket/device';
import { MODES } from '../websocket/constants';
import { DEFAULT_VALUES } from '../websocket/device';
import { websocketManager } from '../websocket/websocketManager';

export const AfterburnerScreen: React.FC = () => {
  const [connected, setConnected] = useState(false);
  const [connecting, setConnecting] = useState(false);
  const [settings, setSettings] = useState<AfterburnerSettings>(DEFAULT_VALUES);
  const [status, setStatus] = useState<DeviceStatus | null>(null);
  const [statusUnsubscribe, setStatusUnsubscribe] = useState<(() => void) | null>(null);
  const [settingsUnsubscribe, setSettingsUnsubscribe] = useState<(() => void) | null>(null);
  const [isSimulator, setIsSimulator] = useState(false);
  const [simulatorWarning, setSimulatorWarning] = useState('');
  const [connectionStatus, setConnectionStatus] = useState({ connected: false, connecting: false });
  
  // Local input states to prevent clearing while typing
  const [speedInput, setSpeedInput] = useState(settings.speedMs.toString());
  const [brightnessInput, setBrightnessInput] = useState(settings.brightness.toString());
  const [numLedsInput, setNumLedsInput] = useState(settings.numLeds.toString());
  const [abThresholdInput, setAbThresholdInput] = useState(settings.abThreshold.toString());

  // Check for simulator mode on component mount
  useEffect(() => {
    const checkSimulator = () => {
      const simulatorMode = websocketManager.isSimulatorMode();
      const warning = websocketManager.getSimulatorWarning();
      setIsSimulator(simulatorMode);
      setSimulatorWarning(warning);
    };
    
    checkSimulator();
    
    // Set up connection status monitoring
    websocketManager.onConnectionStatusChange(setConnectionStatus);
  }, []);

  // Update local input states when settings change
  useEffect(() => {
    setSpeedInput(settings.speedMs.toString());
    setBrightnessInput(settings.brightness.toString());
    setNumLedsInput(settings.numLeds.toString());
    setAbThresholdInput(settings.abThreshold.toString());
  }, [settings.speedMs, settings.brightness, settings.numLeds, settings.abThreshold]);

  // Connect to device
  const handleConnect = async () => {
    if (connected) {
      try {
        await disconnectFromAfterburner();
        setConnected(false);
        if (statusUnsubscribe) {
          statusUnsubscribe();
          setStatusUnsubscribe(null);
        }
      } catch (error) {
        Alert.alert('Error', 'Failed to disconnect');
      }
      return;
    }

    setConnecting(true);
    try {
      console.log('Starting connection process...');
      const success = await connectToAfterburner();
      console.log('Connection success:', success);
      
             if (success) {
         console.log('Setting connected state to true');
         setConnected(true);
        
        try {
          // Read current settings (WebSocket will receive settings from device)
          const currentSettings = await readSettings();
          setSettings(currentSettings);
          
                   // Start monitoring status
         const unsubscribe = await monitorStatus((newStatus) => {
           console.log('Status update received in UI:', newStatus);
           setStatus(newStatus);
         });
          setStatusUnsubscribe(() => unsubscribe);
          
          // Start monitoring settings updates
          const settingsUnsubscribe = await monitorSettings((newSettings) => {
            setSettings(newSettings);
          });
          setSettingsUnsubscribe(() => settingsUnsubscribe);
          
          Alert.alert('Success', 'Connected to Afterburner device via WebSocket');
        } catch (settingsError) {
          console.error('Error setting up monitoring:', settingsError);
          Alert.alert('Warning', 'Connected but failed to set up monitoring. Some features may not work.');
        }
      } else {
        Alert.alert('Error', 'Failed to connect to device. Please check:\n1. ESP8266 device is powered on\n2. Connected to Afterburner_AP WiFi network\n3. Device is running the firmware');
      }
    } catch (error) {
      console.error('Connection error:', error);
      Alert.alert('Error', `Failed to connect to device: ${error instanceof Error ? error.message : 'Unknown error'}`);
    } finally {
      setConnecting(false);
    }
  };

  // Update settings
  const updateSettings = useCallback((newSettings: Partial<AfterburnerSettings>) => {
    setSettings(prev => ({ ...prev, ...newSettings }));
  }, []);

  // All individual send functions removed - only use "Push All Settings"

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



  // Start throttle calibration
  const handleStartCalibration = async () => {
    try {
      await startThrottleCalibration();
      Alert.alert('Success', 'Throttle calibration started! Move your throttle stick from MIN to MAX several times.');
    } catch (error) {
      Alert.alert('Error', `Failed to start calibration: ${error instanceof Error ? error.message : 'Unknown error'}`);
    }
  };

  // Reset throttle calibration
  const handleResetCalibration = async () => {
    try {
      await resetThrottleCalibration();
      Alert.alert('Success', 'Throttle calibration reset! Using default values (900-2100 μs).');
    } catch (error) {
      Alert.alert('Error', `Failed to reset calibration: ${error instanceof Error ? error.message : 'Unknown error'}`);
    }
  };

  // Cleanup on unmount
  useEffect(() => {
    return () => {
      if (statusUnsubscribe) {
        statusUnsubscribe();
      }
      if (settingsUnsubscribe) {
        settingsUnsubscribe();
      }
      if (connected) {
        disconnectFromAfterburner();
      }
    };
  }, [connected, statusUnsubscribe, settingsUnsubscribe]);

  return (
    <ScrollView style={styles.container}>
      <View style={styles.header}>
        <Text style={styles.title}>Afterburner Control</Text>
        <TouchableOpacity
          style={[
            styles.connectButton,
            connected ? styles.disconnectButton : styles.connectButton,
            connecting && styles.connectingButton,
          ]}
          onPress={handleConnect}
          disabled={connecting}
        >
          <Text style={styles.connectButtonText}>
            {connecting ? 'Connecting...' : connected ? 'Disconnect' : 'Connect'}
          </Text>
        </TouchableOpacity>
      </View>

      {/* Simulator Warning */}
      {isSimulator && (
        <View style={styles.simulatorWarning}>
          <Text style={styles.simulatorWarningText}>
            ⚠️ {simulatorWarning}
          </Text>
        </View>
      )}

      {/* Connection Status */}
      {connectionStatus.error && (
        <View style={styles.errorContainer}>
          <Text style={styles.errorText}>
            ❌ {connectionStatus.error}
          </Text>
        </View>
      )}

             {/* Test Mode Indicator */}
       {websocketManager.isTestMode() && (
         <View style={styles.testModeContainer}>
           <Text style={styles.testModeText}>
             🧪 Test Mode Active - Simulating device connection
           </Text>
         </View>
       )}

       {/* Debug Connection Status */}
       <View style={styles.debugContainer}>
         <Text style={styles.debugText}>
           Connected: {connected ? 'Yes' : 'No'} | Status: {status ? 'Received' : 'None'}
         </Text>
       </View>

      {connected && (
        <>
                     {/* Status Display */}
           <View style={styles.statusContainer}>
             <Text style={styles.sectionTitle}>Device Status</Text>
             {status ? (
               <View style={styles.statusInfo}>
                 <Text style={styles.statusText}>
                   Throttle: {(status.throttle * 100).toFixed(1)}%
                 </Text>
                 <Text style={styles.statusText}>
                   Mode: {MODES[status.mode]?.name || 'Unknown'}
                 </Text>
               </View>
             ) : (
               <Text style={styles.statusText}>Waiting for status...</Text>
             )}
           </View>

           {/* Throttle Calibration Status */}
           <View style={styles.statusContainer}>
             <Text style={styles.sectionTitle}>Throttle Calibration</Text>
             {status ? (
               <View style={styles.statusInfo}>
                 <Text style={styles.statusText}>
                   Signal: {status.signalValid ? '✅ Valid' : '❌ Invalid'}
                 </Text>
                 <Text style={styles.statusText}>
                   Pulses: {status.pulseCount} (Invalid: {status.invalidPulseCount})
                 </Text>
                 {status.calibrating && (
                   <Text style={styles.statusText}>
                     🔄 Calibrating... ({status.pulseCount} samples)
                   </Text>
                 )}
                                   {status.calibrationComplete && (
                    <>
                      <Text style={styles.statusText}>
                        ✅ Calibration Complete
                      </Text>
                      <Text style={styles.statusText}>
                        Range: {status.minPulse} - {status.maxPulse} μs
                      </Text>
                      <Text style={styles.statusText}>
                        Span: {status.pulseRange} μs
                      </Text>
                    </>
                  )}
                  {!status.calibrating && !status.calibrationComplete && (
                    <>
                      <Text style={styles.statusText}>
                        ⚠️ Using Default Values
                      </Text>
                      <Text style={styles.statusText}>
                        Range: {status.minPulse} - {status.maxPulse} μs
                      </Text>
                      <Text style={styles.statusText}>
                        Span: {status.pulseRange} μs
                      </Text>
                    </>
                  )}
               </View>
             ) : (
               <Text style={styles.statusText}>Waiting for status...</Text>
             )}
           </View>

          {/* Mode Selection */}
          <View style={styles.section}>
            <Text style={styles.sectionTitle}>Mode</Text>
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
                         <Text style={styles.infoText}>Use "Push All Settings" to update</Text>
          </View>

          {/* Color Controls */}
          <View style={styles.section}>
            <Text style={styles.sectionTitle}>Colors</Text>
                                       <ColorInput
                label="Start Color"
                color={settings.startColor}
                onColorChange={(color) => updateSettings({ startColor: color })}
              />
              <ColorInput
                label="End Color"
                color={settings.endColor}
                onColorChange={(color) => updateSettings({ endColor: color })}
              />
          </View>

                     {/* Speed Control */}
           <View style={styles.section}>
             <Text style={styles.sectionTitle}>Speed (ms) - 100-5000</Text>
             <View style={styles.inputRow}>
               <TextInput
                 style={styles.numberInput}
                 value={speedInput}
                 onChangeText={(text) => {
                   setSpeedInput(text);
                   const value = parseInt(text) || 0;
                   if (value >= 100 && value <= 5000) {
                     updateSettings({ speedMs: value });
                   }
                 }}
                 onBlur={() => {
                   const value = parseInt(speedInput) || 1200;
                   const clampedValue = Math.max(100, Math.min(5000, value));
                   setSpeedInput(clampedValue.toString());
                   updateSettings({ speedMs: clampedValue });
                 }}
                 keyboardType="numeric"
                 placeholder="1200"
               />
                               <Text style={styles.infoText}>Use "Push All Settings" to update</Text>
             </View>
             <Text style={styles.rangeText}>Range: 100-5000 ms</Text>
           </View>

                     {/* Brightness Control */}
           <View style={styles.section}>
             <Text style={styles.sectionTitle}>Brightness - 10-255</Text>
             <View style={styles.inputRow}>
               <TextInput
                 style={styles.numberInput}
                 value={brightnessInput}
                 onChangeText={(text) => {
                   setBrightnessInput(text);
                   const value = parseInt(text) || 0;
                   if (value >= 10 && value <= 255) {
                     updateSettings({ brightness: value });
                   }
                 }}
                 onBlur={() => {
                   const value = parseInt(brightnessInput) || 200;
                   const clampedValue = Math.max(10, Math.min(255, value));
                   setBrightnessInput(clampedValue.toString());
                   updateSettings({ brightness: clampedValue });
                 }}
                 keyboardType="numeric"
                 placeholder="200"
               />
                               <Text style={styles.infoText}>Use "Push All Settings" to update</Text>
             </View>
             <Text style={styles.rangeText}>Range: 10-255</Text>
           </View>

                     {/* Number of LEDs */}
           <View style={styles.section}>
             <Text style={styles.sectionTitle}>Number of LEDs - 1-100</Text>
             <View style={styles.inputRow}>
               <TextInput
                 style={styles.numberInput}
                 value={numLedsInput}
                 onChangeText={(text) => {
                   setNumLedsInput(text);
                   const value = parseInt(text) || 0;
                   if (value >= 1 && value <= 100) {
                     updateSettings({ numLeds: value });
                   }
                 }}
                 onBlur={() => {
                   const value = parseInt(numLedsInput) || 45;
                   const clampedValue = Math.max(1, Math.min(100, value));
                   setNumLedsInput(clampedValue.toString());
                   updateSettings({ numLeds: clampedValue });
                 }}
                 keyboardType="numeric"
                 placeholder="45"
               />
                               <Text style={styles.infoText}>Use "Push All Settings" to update</Text>
             </View>
             <Text style={styles.rangeText}>Range: 1-100 LEDs</Text>
           </View>

                     {/* Afterburner Threshold */}
           <View style={styles.section}>
             <Text style={styles.sectionTitle}>Afterburner Threshold (%) - 0-100</Text>
             <View style={styles.inputRow}>
               <TextInput
                 style={styles.numberInput}
                 value={abThresholdInput}
                 onChangeText={(text) => {
                   setAbThresholdInput(text);
                   const value = parseInt(text) || 0;
                   if (value >= 0 && value <= 100) {
                     updateSettings({ abThreshold: value });
                   }
                 }}
                 onBlur={() => {
                   const value = parseInt(abThresholdInput) || 80;
                   const clampedValue = Math.max(0, Math.min(100, value));
                   setAbThresholdInput(clampedValue.toString());
                   updateSettings({ abThreshold: clampedValue });
                 }}
                 keyboardType="numeric"
                 placeholder="80"
               />
                               <Text style={styles.infoText}>Use "Push All Settings" to update</Text>
             </View>
             <Text style={styles.rangeText}>Range: 0-100%</Text>
           </View>

                                 {/* Action Buttons */}
            <View style={styles.section}>
              <Text style={styles.sectionTitle}>Actions</Text>
              <View style={styles.infoBox}>
                <Text style={styles.infoBoxText}>
                  💡 <Text style={styles.infoBoxBold}>How to use:</Text> Adjust any settings above, then tap "Push All Settings to Device" to send all changes at once. This prevents controller crashes.
                </Text>
              </View>
                           <TouchableOpacity style={[styles.actionButton, styles.primaryActionButton]} onPress={handlePushAll}>
                <Text style={styles.actionButtonText}>🚀 Push All Settings to Device</Text>
              </TouchableOpacity>
                           <TouchableOpacity style={styles.actionButton} onPress={handleSavePreset}>
                <Text style={styles.actionButtonText}>Save Preset</Text>
              </TouchableOpacity>
                           <TouchableOpacity 
                style={[styles.actionButton, status?.calibrating && styles.actionButtonDisabled]} 
                onPress={handleStartCalibration}
                disabled={status?.calibrating}
              >
                <Text style={styles.actionButtonText}>
                  {status?.calibrating ? 'Calibrating...' : 'Start Throttle Calibration'}
                </Text>
              </TouchableOpacity>
              <TouchableOpacity 
                style={[styles.actionButton, status?.calibrating && styles.actionButtonDisabled]} 
                onPress={handleResetCalibration}
                disabled={status?.calibrating}
              >
                <Text style={styles.actionButtonText}>
                  Reset to Default Values
                </Text>
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
  header: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    padding: 16,
    backgroundColor: 'white',
    borderBottomWidth: 1,
    borderBottomColor: '#e0e0e0',
  },
  title: {
    fontSize: 24,
    fontWeight: 'bold',
    color: '#333',
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
  section: {
    margin: 16,
    padding: 16,
    backgroundColor: 'white',
    borderRadius: 8,
    borderWidth: 1,
    borderColor: '#e0e0e0',
  },
  sectionTitle: {
    fontSize: 18,
    fontWeight: '600',
    color: '#333',
    marginBottom: 12,
  },
  statusInfo: {
    gap: 8,
  },
  statusText: {
    fontSize: 16,
    color: '#666',
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
    flexDirection: 'column',
    gap: 8,
  },
  numberInput: {
    height: 44,
    borderWidth: 1,
    borderColor: '#ddd',
    borderRadius: 8,
    paddingHorizontal: 12,
    fontSize: 16,
    backgroundColor: 'white',
  },
  infoText: {
    fontSize: 12,
    color: '#666',
    fontStyle: 'italic',
    textAlign: 'center',
    marginTop: 4,
  },
  rangeText: {
    fontSize: 12,
    color: '#666',
    fontStyle: 'italic',
    marginTop: 4,
  },
  actionButton: {
    backgroundColor: '#17a2b8',
    paddingVertical: 12,
    paddingHorizontal: 20,
    borderRadius: 8,
    marginBottom: 8,
    alignItems: 'center',
  },
  primaryActionButton: {
    backgroundColor: '#28a745',
    paddingVertical: 16,
    marginBottom: 16,
  },
  infoBox: {
    backgroundColor: '#e3f2fd',
    borderColor: '#2196f3',
    borderWidth: 1,
    borderRadius: 8,
    padding: 12,
    marginBottom: 16,
  },
  infoBoxText: {
    fontSize: 14,
    color: '#1565c0',
    lineHeight: 20,
  },
  infoBoxBold: {
    fontWeight: '600',
  },
  actionButtonDisabled: {
    backgroundColor: '#6c757d',
  },
  actionButtonText: {
    color: 'white',
    fontSize: 16,
    fontWeight: '600',
  },
  simulatorWarning: {
    backgroundColor: '#fff3cd',
    borderColor: '#ffeaa7',
    borderWidth: 1,
    borderRadius: 8,
    margin: 16,
    padding: 12,
  },
  simulatorWarningText: {
    color: '#856404',
    fontSize: 14,
    textAlign: 'center',
    fontWeight: '500',
  },
  errorContainer: {
    backgroundColor: '#f8d7da',
    borderColor: '#f5c6cb',
    borderWidth: 1,
    borderRadius: 8,
    margin: 16,
    padding: 12,
  },
  errorText: {
    color: '#721c24',
    fontSize: 14,
    textAlign: 'center',
    fontWeight: '500',
  },
  testModeContainer: {
    backgroundColor: '#d1ecf1',
    borderColor: '#bee5eb',
    borderWidth: 1,
    borderRadius: 8,
    margin: 16,
    padding: 12,
  },
  testModeText: {
    color: '#0c5460',
    fontSize: 14,
    textAlign: 'center',
    fontWeight: '500',
  },
  debugContainer: {
    backgroundColor: '#e9ecef',
    borderColor: '#dee2e6',
    borderWidth: 1,
    borderRadius: 8,
    margin: 16,
    padding: 12,
  },
  debugText: {
    color: '#495057',
    fontSize: 12,
    textAlign: 'center',
    fontWeight: '500',
  },
});
