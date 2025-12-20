#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "settings.h"
#include "constants.h"

// Forward declaration to avoid circular dependency
class ThrottleReader;

// BLE UUIDs
#define SERVICE_UUID "b5f9a000-2b6c-4f6a-93b1-2f1f5f9ab000"
#define MODE_UUID "b5f9a001-2b6c-4f6a-93b1-2f1f5f9ab001"
#define START_COLOR_UUID "b5f9a002-2b6c-4f6a-93b1-2f1f5f9ab002"
#define END_COLOR_UUID "b5f9a003-2b6c-4f6a-93b1-2f1f5f9ab003"
#define SPEED_MS_UUID "b5f9a004-2b6c-4f6a-93b1-2f1f5f9ab004"
#define BRIGHTNESS_UUID "b5f9a005-2b6c-4f6a-93b1-2f1f5f9ab005"
#define NUM_LEDS_UUID "b5f9a006-2b6c-4f6a-93b1-2f1f5f9ab006"
#define AB_THRESHOLD_UUID "b5f9a007-2b6c-4f6a-93b1-2f1f5f9ab007"
#define SAVE_PRESET_UUID "b5f9a008-2b6c-4f6a-93b1-2f1f5f9ab008"
#define STATUS_UUID "b5f9a009-2b6c-4f6a-93b1-2f1f5f9ab009"

// Device name - defined in constants.h

class AfterburnerBLEService {
private:
  BLEServer* pServer;
  BLEService* pService;
  SettingsManager* settingsManager;
  ThrottleReader* throttleReader;  // Reference to throttle reader for calibration updates
  
  // Characteristics
  BLECharacteristic* pModeCharacteristic;
  BLECharacteristic* pStartColorCharacteristic;
  BLECharacteristic* pEndColorCharacteristic;
  BLECharacteristic* pSpeedMsCharacteristic;
  BLECharacteristic* pBrightnessCharacteristic;
  BLECharacteristic* pNumLedsCharacteristic;
  BLECharacteristic* pAbThresholdCharacteristic;
  BLECharacteristic* pSavePresetCharacteristic;
  BLECharacteristic* pStatusCharacteristic;
  
  // Throttle calibration characteristics
  BLECharacteristic* pThrottleCalibrationCharacteristic;
  BLECharacteristic* pThrottleCalibrationStatusCharacteristic;
  BLECharacteristic* pThrottleCalibrationResetCharacteristic;
  
  // Hardware version characteristic (optional, for auto-detection)
  BLECharacteristic* pHardwareVersionCharacteristic;
  
  // Status notification timer
  unsigned long lastStatusUpdate;
  
public:
  // Connection state - made public for callback access
  bool deviceConnected;
  
  AfterburnerBLEService(SettingsManager* settings, ThrottleReader* throttle);
  void begin();
  void updateStatus(float throttle, uint8_t mode);
  void updateThrottleCalibrationStatus(bool isCalibrated, uint16_t minPWM, uint16_t maxPWM);
  void updateThrottleCalibrationProgress(uint16_t minPWM, uint16_t maxPWM, uint8_t minVisits, uint8_t maxVisits);
  void notifyCalibrationStatus();
  SettingsManager* getSettingsManager() { return settingsManager; }
  bool isConnected();
  void restartAdvertising();
  bool isAdvertising();
  void ensureAdvertising();
  
  // Make callback methods public so callback classes can access them
  void handleModeWrite(BLECharacteristic* pCharacteristic);
  void handleStartColorWrite(BLECharacteristic* pCharacteristic);
  void handleEndColorWrite(BLECharacteristic* pCharacteristic);
  void handleSpeedMsWrite(BLECharacteristic* pCharacteristic);
  void handleBrightnessWrite(BLECharacteristic* pCharacteristic);
  void handleNumLedsWrite(BLECharacteristic* pCharacteristic);
  void handleAbThresholdWrite(BLECharacteristic* pCharacteristic);
  void handleSavePresetWrite(BLECharacteristic* pCharacteristic);
  
  // Throttle calibration handlers
  void handleThrottleCalibrationWrite(BLECharacteristic* pCharacteristic);
  void handleThrottleCalibrationResetWrite(BLECharacteristic* pCharacteristic);
  
private:
  void createService();
  void setupCallbacks();
  void updateCharacteristicValues();
  uint16_t bytesToUint16(const uint8_t* data);
  void uint16ToBytes(uint16_t value, uint8_t* data);
};

#endif // BLE_SERVICE_H
