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
  AfterburnerSettings,
  DeviceStatus,
} from '../ble/device';
import { MODES, DEFAULT_VALUES } from '../ble/uuids';
import { bleManager } from '../ble/bleManager';

export const AfterburnerScreen: React.FC = () => {
  const [connected, setConnected] = useState(false);
  const [connecting, setConnecting] = useState(false);
  const [settings, setSettings] = useState<AfterburnerSettings>(DEFAULT_VALUES);
  const [status, setStatus] = useState<DeviceStatus | null>(null);
  const [statusUnsubscribe, setStatusUnsubscribe] = useState<(() => void) | null>(null);
  const [isSimulator, setIsSimulator] = useState(false);
  const [simulatorWarning, setSimulatorWarning] = useState('');

  // Check for simulator mode on component mount
  useEffect(() => {
    const checkSimulator = () => {
      const simulatorMode = bleManager.isSimulatorMode();
      const warning = bleManager.getSimulatorWarning();
      setIsSimulator(simulatorMode);
      setSimulatorWarning(warning);
    };
    
    checkSimulator();
  }, []);

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
      const success = await connectToAfterburner();
      if (success) {
        setConnected(true);
        
        // Read current settings
        const currentSettings = await readSettings();
        setSettings(currentSettings);
        
        // Start monitoring status
        const unsubscribe = await monitorStatus((newStatus) => {
          setStatus(newStatus);
        });
        setStatusUnsubscribe(() => unsubscribe);
        
        Alert.alert('Success', 'Connected to Afterburner device');
      } else {
        Alert.alert('Error', 'Failed to connect to device');
      }
    } catch (error) {
      Alert.alert('Error', 'Failed to connect to device');
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

  // Cleanup on unmount
  useEffect(() => {
    return () => {
      if (statusUnsubscribe) {
        statusUnsubscribe();
      }
      if (connected) {
        disconnectFromAfterburner();
      }
    };
  }, [connected, statusUnsubscribe]);

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
              onColorChange={(color) => updateSettings({ startColor: color })}
              onSend={sendStartColor}
            />
            <ColorInput
              label="End Color"
              color={settings.endColor}
              onColorChange={(color) => updateSettings({ endColor: color })}
              onSend={sendEndColor}
            />
          </View>

          {/* Speed Control */}
          <View style={styles.section}>
            <Text style={styles.sectionTitle}>Speed (ms) - 100-5000</Text>
            <View style={styles.inputRow}>
              <TextInput
                style={styles.numberInput}
                value={settings.speedMs.toString()}
                onChangeText={(text) => {
                  const value = parseInt(text) || 0;
                  const clampedValue = Math.max(100, Math.min(5000, value));
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
                value={settings.brightness.toString()}
                onChangeText={(text) => {
                  const value = parseInt(text) || 0;
                  const clampedValue = Math.max(10, Math.min(255, value));
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
                value={settings.numLeds.toString()}
                onChangeText={(text) => {
                  const value = parseInt(text) || 0;
                  const clampedValue = Math.max(1, Math.min(100, value));
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
                value={settings.abThreshold.toString()}
                onChangeText={(text) => {
                  const value = parseInt(text) || 0;
                  const clampedValue = Math.max(0, Math.min(100, value));
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
});
