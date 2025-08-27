#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H

#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include "settings.h"

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

// Device name
#define DEVICE_NAME "ABurner"

class AfterburnerBLEService {
private:
  NimBLEServer* pServer;
  NimBLEService* pService;
  SettingsManager* settingsManager;
  
  // Characteristics
  NimBLECharacteristic* pModeCharacteristic;
  NimBLECharacteristic* pStartColorCharacteristic;
  NimBLECharacteristic* pEndColorCharacteristic;
  NimBLECharacteristic* pSpeedMsCharacteristic;
  NimBLECharacteristic* pBrightnessCharacteristic;
  NimBLECharacteristic* pNumLedsCharacteristic;
  NimBLECharacteristic* pAbThresholdCharacteristic;
  NimBLECharacteristic* pSavePresetCharacteristic;
  NimBLECharacteristic* pStatusCharacteristic;
  
  // Status notification timer
  unsigned long lastStatusUpdate;
  
public:
  AfterburnerBLEService(SettingsManager* settings);
  void begin();
  void updateStatus(float throttle, uint8_t mode);
  bool isConnected();
  
  // Make callback methods public so callback classes can access them
  void handleModeWrite(NimBLECharacteristic* pCharacteristic);
  void handleStartColorWrite(NimBLECharacteristic* pCharacteristic);
  void handleEndColorWrite(NimBLECharacteristic* pCharacteristic);
  void handleSpeedMsWrite(NimBLECharacteristic* pCharacteristic);
  void handleBrightnessWrite(NimBLECharacteristic* pCharacteristic);
  void handleNumLedsWrite(NimBLECharacteristic* pCharacteristic);
  void handleAbThresholdWrite(NimBLECharacteristic* pCharacteristic);
  void handleSavePresetWrite(NimBLECharacteristic* pCharacteristic);
  
private:
  void createService();
  void setupCallbacks();
  void updateCharacteristicValues();
  uint16_t bytesToUint16(const uint8_t* data);
  void uint16ToBytes(uint16_t value, uint8_t* data);
};

#endif // BLE_SERVICE_H
