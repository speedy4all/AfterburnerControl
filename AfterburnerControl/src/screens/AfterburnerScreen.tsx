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
  monitorStatus,
  monitorSettings,
  sendPing,
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
      await writeSpeedMs(settings.speedMs);
      Alert.alert('Success', 'Speed updated');
    } catch (error) {
      Alert.alert('Error', 'Failed to update speed');
    }
  };

  const sendBrightness = async () => {
    try {
      await writeBrightness(settings.brightness);
      Alert.alert('Success', 'Brightness updated');
    } catch (error) {
      Alert.alert('Error', 'Failed to update brightness');
    }
  };

  const sendNumLeds = async () => {
    try {
      await writeNumLeds(settings.numLeds);
      Alert.alert('Success', 'Number of LEDs updated');
    } catch (error) {
      Alert.alert('Error', 'Failed to update number of LEDs');
    }
  };

  const sendAbThreshold = async () => {
    try {
      await writeAbThreshold(settings.abThreshold);
      Alert.alert('Success', 'Afterburner threshold updated');
    } catch (error) {
      Alert.alert('Error', 'Failed to update afterburner threshold');
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

  // Test ping
  const handlePing = async () => {
    try {
      await sendPing();
      Alert.alert('Success', 'Ping sent to device');
    } catch (error) {
      Alert.alert('Error', 'Failed to send ping');
    }
  };

  // Test connection (for debugging)
  const handleTestConnection = async () => {
    try {
      const result = await websocketManager.testConnection();
      if (result.success) {
        Alert.alert('Success', 'Connection test successful!');
      } else {
        Alert.alert('Error', `Connection test failed: ${result.error}`);
      }
    } catch (error) {
      Alert.alert('Error', `Connection test error: ${error instanceof Error ? error.message : 'Unknown error'}`);
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
            <TouchableOpacity style={styles.sendButton} onPress={sendMode}>
              <Text style={styles.sendButtonText}>Send Mode</Text>
            </TouchableOpacity>
          </View>

          {/* Color Controls */}
          <View style={styles.section}>
            <Text style={styles.sectionTitle}>Colors</Text>
                         <ColorInput
               label="Start Color"
               color={settings.startColor}
               onColorChange={(color) => updateSettings({ startColor: [color.r, color.g, color.b] })}
               onSend={sendStartColor}
             />
             <ColorInput
               label="End Color"
               color={settings.endColor}
               onColorChange={(color) => updateSettings({ endColor: [color.r, color.g, color.b] })}
               onSend={sendEndColor}
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
               <TouchableOpacity style={styles.sendButton} onPress={sendSpeed}>
                 <Text style={styles.sendButtonText}>Send</Text>
               </TouchableOpacity>
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
               <TouchableOpacity style={styles.sendButton} onPress={sendBrightness}>
                 <Text style={styles.sendButtonText}>Send</Text>
               </TouchableOpacity>
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
               <TouchableOpacity style={styles.sendButton} onPress={sendNumLeds}>
                 <Text style={styles.sendButtonText}>Send</Text>
               </TouchableOpacity>
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
               <TouchableOpacity style={styles.sendButton} onPress={sendAbThreshold}>
                 <Text style={styles.sendButtonText}>Send</Text>
               </TouchableOpacity>
             </View>
             <Text style={styles.rangeText}>Range: 0-100%</Text>
           </View>

          {/* Action Buttons */}
          <View style={styles.section}>
            <Text style={styles.sectionTitle}>Actions</Text>
            <TouchableOpacity style={styles.actionButton} onPress={handlePushAll}>
              <Text style={styles.actionButtonText}>Push All Settings</Text>
            </TouchableOpacity>
            <TouchableOpacity style={styles.actionButton} onPress={handleSavePreset}>
              <Text style={styles.actionButtonText}>Save Preset</Text>
            </TouchableOpacity>
                         <TouchableOpacity style={styles.actionButton} onPress={handlePing}>
               <Text style={styles.actionButtonText}>Test Ping</Text>
             </TouchableOpacity>
             <TouchableOpacity style={styles.actionButton} onPress={handleTestConnection}>
               <Text style={styles.actionButtonText}>Test Connection</Text>
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
    flexDirection: 'row',
    alignItems: 'center',
    gap: 12,
  },
  numberInput: {
    flex: 1,
    height: 44,
    borderWidth: 1,
    borderColor: '#ddd',
    borderRadius: 8,
    paddingHorizontal: 12,
    fontSize: 16,
    backgroundColor: 'white',
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
