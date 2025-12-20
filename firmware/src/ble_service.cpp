#include "ble_service.h"
#include <ArduinoJson.h>
#include "throttle.h"
#include "constants.h"

// Forward declaration for throttle calibration
extern void startThrottleCalibration();

// Server callbacks for connection monitoring
class ServerCallbacks : public BLEServerCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  ServerCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  
  void onConnect(BLEServer* pServer) {
    Serial.println("BLE: Client connected successfully!");
    bleService->deviceConnected = true;
    
    // Send current calibration status to newly connected client
    delay(500); // Give BLE stack time to establish connection
    if (bleService->getSettingsManager()) {
      bool isCalibrated = bleService->getSettingsManager()->isThrottleCalibrated();
      uint16_t minPWM = bleService->getSettingsManager()->getThrottleMin();
      uint16_t maxPWM = bleService->getSettingsManager()->getThrottleMax();
      bleService->updateThrottleCalibrationStatus(isCalibrated, minPWM, maxPWM);
    }
  }
  
  void onDisconnect(BLEServer* pServer) {
    Serial.println("BLE: Client disconnected");
    bleService->deviceConnected = false;
    
    // Automatically restart advertising when client disconnects
    if (bleService) {
      bleService->restartAdvertising();
    }
  }
};

// Callback classes for BLE characteristics
class ModeCharacteristicCallbacks : public BLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  ModeCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(BLECharacteristic* pCharacteristic) {
    bleService->handleModeWrite(pCharacteristic);
  }
};

class StartColorCharacteristicCallbacks : public BLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  StartColorCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(BLECharacteristic* pCharacteristic) {
    bleService->handleStartColorWrite(pCharacteristic);
  }
};

class EndColorCharacteristicCallbacks : public BLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  EndColorCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(BLECharacteristic* pCharacteristic) {
    bleService->handleEndColorWrite(pCharacteristic);
  }
};

class SpeedMsCharacteristicCallbacks : public BLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  SpeedMsCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(BLECharacteristic* pCharacteristic) {
    bleService->handleSpeedMsWrite(pCharacteristic);
  }
};

class BrightnessCharacteristicCallbacks : public BLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  BrightnessCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(BLECharacteristic* pCharacteristic) {
    bleService->handleBrightnessWrite(pCharacteristic);
  }
};

class NumLedsCharacteristicCallbacks : public BLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  NumLedsCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(BLECharacteristic* pCharacteristic) {
    bleService->handleNumLedsWrite(pCharacteristic);
  }
};

class AbThresholdCharacteristicCallbacks : public BLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  AbThresholdCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(BLECharacteristic* pCharacteristic) {
    bleService->handleAbThresholdWrite(pCharacteristic);
  }
};

class SavePresetCharacteristicCallbacks : public BLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  SavePresetCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(BLECharacteristic* pCharacteristic) {
    bleService->handleSavePresetWrite(pCharacteristic);
  }
};

// Throttle calibration callback classes
class ThrottleCalibrationCharacteristicCallbacks : public BLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  ThrottleCalibrationCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(BLECharacteristic* pCharacteristic) {
    bleService->handleThrottleCalibrationWrite(pCharacteristic);
  }
};

class ThrottleCalibrationResetCharacteristicCallbacks : public BLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  ThrottleCalibrationResetCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(BLECharacteristic* pCharacteristic) {
    bleService->handleThrottleCalibrationResetWrite(pCharacteristic);
  }
};

AfterburnerBLEService::AfterburnerBLEService(SettingsManager* settings, ThrottleReader* throttle) {
  settingsManager = settings;
  throttleReader = throttle;
  pServer = nullptr;
  pService = nullptr;
  lastStatusUpdate = 0;
  deviceConnected = false;
  
  // Initialize characteristics to nullptr
  pModeCharacteristic = nullptr;
  pStartColorCharacteristic = nullptr;
  pEndColorCharacteristic = nullptr;
  pSpeedMsCharacteristic = nullptr;
  pBrightnessCharacteristic = nullptr;
  pNumLedsCharacteristic = nullptr;
  pAbThresholdCharacteristic = nullptr;
  pSavePresetCharacteristic = nullptr;
  pStatusCharacteristic = nullptr;
  
  // Initialize throttle calibration characteristics to nullptr
  pThrottleCalibrationCharacteristic = nullptr;
  pThrottleCalibrationStatusCharacteristic = nullptr;
  pThrottleCalibrationResetCharacteristic = nullptr;
  
  // Initialize hardware version characteristic to nullptr
  pHardwareVersionCharacteristic = nullptr;
}

void AfterburnerBLEService::begin() {
  Serial.println("BLE: Starting BLE initialization...");
  
  // Initialize BLE device
  BLEDevice::init(DEVICE_NAME);
  
  // Give BLE stack a moment to initialize
  delay(100);
  
  // Create BLE server
  pServer = BLEDevice::createServer();
  
  // Set server callbacks for connection monitoring
  pServer->setCallbacks(new ServerCallbacks(this));
  
  // Create service
  createService();
  
  // Start advertising
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  
  pAdvertising->addServiceUUID(SERVICE_UUID);
  
  // Set device name in advertising data
  pAdvertising->setName(DEVICE_NAME);
  
  // Set advertising parameters for better compatibility
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  
  // Start advertising
  BLEDevice::startAdvertising();
  
  Serial.println("BLE service started successfully");
}

void AfterburnerBLEService::createService() {
  pService = pServer->createService(SERVICE_UUID);
  if (!pService) {
    Serial.println("ERROR: Failed to create BLE service!");
    return;
  }
  
  // Create characteristics
  
  pModeCharacteristic = pService->createCharacteristic(
    MODE_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );
  if (!pModeCharacteristic) {
    Serial.println("ERROR: Failed to create mode characteristic!");
    return;
  }
  Serial.printf("BLE: Mode characteristic created - UUID: %s\n", MODE_UUID);
  Serial.printf("BLE: Mode characteristic properties: READ=YES, WRITE=YES\n");
  
  pStartColorCharacteristic = pService->createCharacteristic(
    START_COLOR_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );
  if (!pStartColorCharacteristic) {
    Serial.println("ERROR: Failed to create start color characteristic!");
    return;
  }
  Serial.printf("BLE: Start color characteristic created - UUID: %s\n", START_COLOR_UUID);
  
  pEndColorCharacteristic = pService->createCharacteristic(
    END_COLOR_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );
  if (!pEndColorCharacteristic) {
    Serial.println("ERROR: Failed to create end color characteristic!");
    return;
  }
  Serial.printf("BLE: End color characteristic created - UUID: %s\n", END_COLOR_UUID);
  
  pSpeedMsCharacteristic = pService->createCharacteristic(
    SPEED_MS_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );
  if (!pSpeedMsCharacteristic) {
    Serial.println("ERROR: Failed to create speed characteristic!");
    return;
  }
  Serial.printf("BLE: Speed characteristic created - UUID: %s\n", SPEED_MS_UUID);
  
  pBrightnessCharacteristic = pService->createCharacteristic(
    BRIGHTNESS_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );
  if (!pBrightnessCharacteristic) {
    Serial.println("ERROR: Failed to create brightness characteristic!");
    return;
  }
  Serial.printf("BLE: Brightness characteristic created - UUID: %s\n", BRIGHTNESS_UUID);
  
  pNumLedsCharacteristic = pService->createCharacteristic(
    NUM_LEDS_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );
  if (!pNumLedsCharacteristic) {
    Serial.println("ERROR: Failed to create num LEDs characteristic!");
    return;
  }
  Serial.printf("BLE: Num LEDs characteristic created - UUID: %s\n", NUM_LEDS_UUID);
  
  pAbThresholdCharacteristic = pService->createCharacteristic(
    AB_THRESHOLD_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );
  if (!pAbThresholdCharacteristic) {
    Serial.println("ERROR: Failed to create AB threshold characteristic!");
    return;
  }
  Serial.printf("BLE: AB Threshold characteristic created - UUID: %s\n", AB_THRESHOLD_UUID);
  
  pSavePresetCharacteristic = pService->createCharacteristic(
    SAVE_PRESET_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  if (!pSavePresetCharacteristic) {
    Serial.println("ERROR: Failed to create save preset characteristic!");
    return;
  }
  Serial.printf("BLE: Save Preset characteristic created - UUID: %s\n", SAVE_PRESET_UUID);
  
  // Create throttle calibration characteristics
  pThrottleCalibrationCharacteristic = pService->createCharacteristic(
    THROTTLE_CALIBRATION_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  if (!pThrottleCalibrationCharacteristic) {
    Serial.println("ERROR: Failed to create throttle calibration characteristic!");
    return;
  }
  Serial.printf("BLE: Throttle calibration characteristic created - UUID: %s\n", THROTTLE_CALIBRATION_UUID);
  
  pThrottleCalibrationStatusCharacteristic = pService->createCharacteristic(
    THROTTLE_CALIBRATION_STATUS_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  if (!pThrottleCalibrationStatusCharacteristic) {
    Serial.println("ERROR: Failed to create throttle calibration status characteristic!");
    return;
  }
  Serial.printf("BLE: Throttle calibration status characteristic created - UUID: %s\n", THROTTLE_CALIBRATION_STATUS_UUID);
  
  // Add descriptor for notifications (required for ESP32 BLE)
  pThrottleCalibrationStatusCharacteristic->addDescriptor(new BLE2902());
  
  pThrottleCalibrationResetCharacteristic = pService->createCharacteristic(
    THROTTLE_CALIBRATION_RESET_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  if (!pThrottleCalibrationResetCharacteristic) {
    Serial.println("ERROR: Failed to create throttle calibration reset characteristic!");
    return;
  }
  Serial.printf("BLE: Throttle calibration reset characteristic created - UUID: %s\n", THROTTLE_CALIBRATION_RESET_UUID);
  
  // Create hardware version characteristic (optional, for auto-detection)
  pHardwareVersionCharacteristic = pService->createCharacteristic(
    HARDWARE_VERSION_UUID,
    BLECharacteristic::PROPERTY_READ
  );
  if (!pHardwareVersionCharacteristic) {
    Serial.println("ERROR: Failed to create hardware version characteristic!");
    // Non-critical, continue anyway
  } else {
    Serial.printf("BLE: Hardware version characteristic created - UUID: %s\n", HARDWARE_VERSION_UUID);
  }
  
  pStatusCharacteristic = pService->createCharacteristic(
    STATUS_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  if (!pStatusCharacteristic) {
    Serial.println("ERROR: Failed to create status characteristic!");
    return;
  }
  Serial.printf("BLE: Status characteristic created - UUID: %s\n", STATUS_UUID);
  
  // Add descriptor for notifications (required for ESP32 BLE)
  pStatusCharacteristic->addDescriptor(new BLE2902());
  
  Serial.println("BLE: All characteristics created successfully");
  
  // Setup callbacks BEFORE starting the service
  Serial.println("BLE: Setting up callbacks BEFORE starting service...");
  setupCallbacks();
  Serial.println("BLE: Callbacks setup completed");
  
  // Start the service AFTER callbacks are set up
  pService->start();
  Serial.println("BLE: Service started");
  
  // Verify write properties are set correctly
  uint16_t modeProps = pModeCharacteristic->getProperties();
  Serial.printf("BLE: Mode characteristic properties: READ=%s, WRITE=%s, NOTIFY=%s\n",
                (modeProps & BLECharacteristic::PROPERTY_READ) ? "YES" : "NO",
                (modeProps & BLECharacteristic::PROPERTY_WRITE) ? "YES" : "NO",
                (modeProps & BLECharacteristic::PROPERTY_NOTIFY) ? "YES" : "NO");
  
  // Test if the characteristic can actually be written to
  Serial.println("BLE: Testing characteristic write capability...");
  uint8_t testValue = 99;
  pModeCharacteristic->setValue(&testValue, 1);
  Serial.println("BLE: Test write completed");
  
  // Set initial values
  updateCharacteristicValues();
  Serial.println("BLE: Initial characteristic values set");
}

void AfterburnerBLEService::setupCallbacks() {
  Serial.println("BLE: Setting up callbacks...");
  
  // Set up each callback with error checking
  if (pModeCharacteristic) {
    pModeCharacteristic->setCallbacks(new ModeCharacteristicCallbacks(this));
    Serial.println("BLE: ✅ Mode callbacks set");
  } else {
    Serial.println("BLE: ❌ ERROR - Mode characteristic is null!");
  }
  
  if (pStartColorCharacteristic) {
    pStartColorCharacteristic->setCallbacks(new StartColorCharacteristicCallbacks(this));
    Serial.println("BLE: ✅ Start color callbacks set");
  } else {
    Serial.println("BLE: ❌ ERROR - Start color characteristic is null!");
  }
  
  if (pEndColorCharacteristic) {
    pEndColorCharacteristic->setCallbacks(new EndColorCharacteristicCallbacks(this));
    Serial.println("BLE: ✅ End color callbacks set");
  } else {
    Serial.println("BLE: ❌ ERROR - End color characteristic is null!");
  }
  
  if (pSpeedMsCharacteristic) {
    pSpeedMsCharacteristic->setCallbacks(new SpeedMsCharacteristicCallbacks(this));
    Serial.println("BLE: ✅ Speed callbacks set");
  } else {
    Serial.println("BLE: ❌ ERROR - Speed characteristic is null!");
  }
  
  if (pBrightnessCharacteristic) {
    pBrightnessCharacteristic->setCallbacks(new BrightnessCharacteristicCallbacks(this));
    Serial.println("BLE: ✅ Brightness callbacks set");
  } else {
    Serial.println("BLE: ❌ ERROR - Brightness characteristic is null!");
  }
  
  if (pNumLedsCharacteristic) {
    pNumLedsCharacteristic->setCallbacks(new NumLedsCharacteristicCallbacks(this));
    Serial.println("BLE: ✅ Num LEDs callbacks set");
  } else {
    Serial.println("BLE: ❌ ERROR - Num LEDs characteristic is null!");
  }
  
  if (pAbThresholdCharacteristic) {
    pAbThresholdCharacteristic->setCallbacks(new AbThresholdCharacteristicCallbacks(this));
    Serial.println("BLE: ✅ AB threshold callbacks set");
  } else {
    Serial.println("BLE: ❌ ERROR - AB threshold characteristic is null!");
  }
  
  if (pSavePresetCharacteristic) {
    pSavePresetCharacteristic->setCallbacks(new SavePresetCharacteristicCallbacks(this));
    Serial.println("BLE: ✅ Save preset callbacks set");
  } else {
    Serial.println("BLE: ❌ ERROR - Save preset characteristic is null!");
  }
  
  // Set up throttle calibration callbacks
  if (pThrottleCalibrationCharacteristic) {
    pThrottleCalibrationCharacteristic->setCallbacks(new ThrottleCalibrationCharacteristicCallbacks(this));
    Serial.println("BLE: ✅ Throttle calibration callbacks set");
  } else {
    Serial.println("BLE: ❌ ERROR - Throttle calibration characteristic is null!");
  }
  
  if (pThrottleCalibrationResetCharacteristic) {
    pThrottleCalibrationResetCharacteristic->setCallbacks(new ThrottleCalibrationResetCharacteristicCallbacks(this));
    Serial.println("BLE: ✅ Throttle calibration reset callbacks set");
  } else {
    Serial.println("BLE: ❌ ERROR - Throttle calibration reset characteristic is null!");
  }
  
  Serial.println("BLE: All callbacks setup completed successfully");
}

void AfterburnerBLEService::updateCharacteristicValues() {
  Serial.println("BLE: Setting initial characteristic values...");
  
  AfterburnerSettings& settings = settingsManager->getSettings();
  
  // Set initial values
  pModeCharacteristic->setValue(&settings.mode, 1);
  Serial.printf("BLE: Mode characteristic set to: %d\n", settings.mode);
  
  pStartColorCharacteristic->setValue(settings.startColor, 3);
  Serial.printf("BLE: Start color characteristic set to: R%d G%d B%d\n", 
                settings.startColor[0], settings.startColor[1], settings.startColor[2]);
  
  pEndColorCharacteristic->setValue(settings.endColor, 3);
  Serial.printf("BLE: End color characteristic set to: R%d G%d B%d\n", 
                settings.endColor[0], settings.endColor[1], settings.endColor[2]);
  
  uint8_t speedBytes[2];
  uint16ToBytes(settings.speedMs, speedBytes);
  pSpeedMsCharacteristic->setValue(speedBytes, 2);
  Serial.printf("BLE: Speed characteristic set to: %dms\n", settings.speedMs);
  
  pBrightnessCharacteristic->setValue(&settings.brightness, 1);
  Serial.printf("BLE: Brightness characteristic set to: %d\n", settings.brightness);
  
  // Set numLeds to fixed value (36) for new hardware
  // This allows apps to detect hardware type: 36 or 0 = new hardware, 1-300 = legacy
  uint16_t numLedsValue = TOTAL_LEDS;  // Fixed at 36 for new hardware
  uint8_t numLedsBytes[2];
  uint16ToBytes(numLedsValue, numLedsBytes);
  pNumLedsCharacteristic->setValue(numLedsBytes, 2);
  Serial.printf("BLE: Num LEDs characteristic set to: %d (fixed for new hardware)\n", numLedsValue);
  
  // Set hardware version to 2 (new hardware)
  if (pHardwareVersionCharacteristic) {
    uint8_t hardwareVersion = 2;  // 1 = Legacy, 2 = New hardware
    pHardwareVersionCharacteristic->setValue(&hardwareVersion, 1);
    Serial.printf("BLE: Hardware version characteristic set to: %d (new hardware)\n", hardwareVersion);
  }
  
  pAbThresholdCharacteristic->setValue(&settings.abThreshold, 1);
  Serial.printf("BLE: AB Threshold characteristic set to: %d%%\n", settings.abThreshold);
  
  Serial.println("BLE: All characteristic values set successfully");
  
  // Verify the characteristics are accessible
  Serial.println("BLE: Verifying characteristic accessibility...");
  if (pModeCharacteristic->getValue().length() > 0) {
    Serial.println("BLE: Mode characteristic is accessible");
  } else {
    Serial.println("BLE: ERROR - Mode characteristic not accessible");
  }
}

void AfterburnerBLEService::updateStatus(float throttle, uint8_t mode) {
  // Send status notification every 200ms
  if (millis() - lastStatusUpdate > 200) {
    // Create JSON status with much larger buffer to prevent truncation
    StaticJsonDocument<256> doc;
    doc["thr"] = round(throttle * 100) / 100.0; // Round to 2 decimal places
    doc["mode"] = mode;
    
    String statusJson;
    serializeJson(doc, statusJson);
    
    // Verify JSON is complete before sending
    if (statusJson.length() > 0 && statusJson.endsWith("}")) {
      // Set the value and notify
      pStatusCharacteristic->setValue(statusJson.c_str());
      pStatusCharacteristic->notify();
      
      lastStatusUpdate = millis();
      
      // Log status updates every 5 seconds to avoid spam
      static unsigned long lastStatusLog = 0;
      if (millis() - lastStatusLog > 5000) {
        Serial.printf("BLE: Status sent - Throttle: %.1f%%, Mode: %d, JSON: '%s'\n", 
                      throttle * 100, mode, statusJson.c_str());
        lastStatusLog = millis();
      }
    } else {
      Serial.printf("BLE: ERROR - Invalid JSON generated: '%s' (length: %d)\n", 
                    statusJson.c_str(), statusJson.length());
    }
  }
}

void AfterburnerBLEService::updateThrottleCalibrationStatus(bool isCalibrated, uint16_t minPWM, uint16_t maxPWM) {
  if (!pThrottleCalibrationStatusCharacteristic) {
    Serial.println("BLE: ❌ Throttle calibration status characteristic not available");
    return;
  }
  
  // Create status data: [isCalibrated: 1 byte, min: 2 bytes, max: 2 bytes]
  uint8_t statusData[5];
  statusData[0] = isCalibrated ? 1 : 0;
  uint16ToBytes(minPWM, &statusData[1]);
  uint16ToBytes(maxPWM, &statusData[3]);
  
  // Update the characteristic value
  pThrottleCalibrationStatusCharacteristic->setValue(statusData, 5);
  
  // Send notification to connected clients
  pThrottleCalibrationStatusCharacteristic->notify();
  
  Serial.printf("BLE: 🎯 Throttle calibration status updated and notified - Calibrated: %s, Min: %u, Max: %u\n",
                isCalibrated ? "true" : "false", minPWM, maxPWM);
}

void AfterburnerBLEService::updateThrottleCalibrationProgress(uint16_t minPWM, uint16_t maxPWM, uint8_t minVisits, uint8_t maxVisits) {
  if (!pThrottleCalibrationStatusCharacteristic) {
    Serial.println("BLE: ❌ Throttle calibration status characteristic not available");
    return;
  }
  
  // Create progress data: [isCalibrated: 1 byte, min: 2 bytes, max: 2 bytes, minVisits: 1 byte, maxVisits: 1 byte]
  uint8_t progressData[7];
  progressData[0] = 0; // Not calibrated yet
  uint16ToBytes(minPWM, &progressData[1]);
  uint16ToBytes(maxPWM, &progressData[3]);
  progressData[5] = minVisits;
  progressData[6] = maxVisits;
  
  // Update the characteristic value
  pThrottleCalibrationStatusCharacteristic->setValue(progressData, 7);
  
  // Send notification to connected clients
  pThrottleCalibrationStatusCharacteristic->notify();
  
  Serial.printf("BLE: 🎯 Calibration progress updated - Min: %u, Max: %u, Min visits: %d/%d, Max visits: %d/%d\n",
                minPWM, maxPWM, minVisits, MIN_VISITS_REQUIRED, maxVisits, MAX_VISITS_REQUIRED);
}

void AfterburnerBLEService::notifyCalibrationStatus() {
  if (!pThrottleCalibrationStatusCharacteristic) {
    Serial.println("BLE: ❌ Throttle calibration status characteristic not available");
    return;
  }
  
  // Send a notification to trigger the app to read the current status
  pThrottleCalibrationStatusCharacteristic->notify();
  Serial.println("BLE: 🎯 Calibration status notification sent");
}

bool AfterburnerBLEService::isConnected() {
  return deviceConnected;
}

void AfterburnerBLEService::restartAdvertising() {
  Serial.println("BLE: Restarting advertising...");
  
  // Stop current advertising if it's active
  if (BLEDevice::getAdvertising()->isAdvertising()) {
    BLEDevice::stopAdvertising();
    Serial.println("BLE: Current advertising stopped");
    
    // Wait a moment for cleanup
    delay(100);
  }
  
  // Get advertising object and reconfigure
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  
  // Add our service UUID (it's safe to add multiple times)
  pAdvertising->addServiceUUID(SERVICE_UUID);
  
  // Set device name
  pAdvertising->setName(DEVICE_NAME);
  
  // Set advertising parameters for better compatibility
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  
  // Start advertising again
  BLEDevice::startAdvertising();
  
  // Verify advertising started successfully
  delay(50); // Give advertising a moment to start
  if (pAdvertising->isAdvertising()) {
    Serial.println("BLE: Advertising restarted successfully");
    Serial.printf("BLE: Device name: %s\n", DEVICE_NAME);
    Serial.printf("BLE: Service UUID: %s\n", SERVICE_UUID);
  } else {
    Serial.println("BLE: ERROR - Failed to start advertising!");
  }
}

bool AfterburnerBLEService::isAdvertising() {
  if (pServer) {
    // Check if advertising is currently active
    return BLEDevice::getAdvertising()->isAdvertising();
  }
  return false;
}

void AfterburnerBLEService::ensureAdvertising() {
  // Only ensure advertising if we're not connected
  if (!isConnected()) {
    if (!isAdvertising()) {
      Serial.println("BLE: Ensuring advertising is active...");
      restartAdvertising();
    }
  }
}

void AfterburnerBLEService::handleModeWrite(BLECharacteristic* pCharacteristic) {
  Serial.println("BLE: 🎯 handleModeWrite called!");
  String value = pCharacteristic->getValue();
  Serial.printf("BLE: Mode write received - length: %d, value: %d\n", value.length(), value.length() > 0 ? value.charAt(0) : -1);
  
  if (value.length() == 1) {
    uint8_t mode = value.charAt(0);
    Serial.printf("BLE: Processing mode value: %d\n", mode);
    
    if (mode <= 2) {
      AfterburnerSettings& settings = settingsManager->getSettings();
      uint8_t oldMode = settings.mode;
      Serial.printf("BLE: Current mode in settings: %d\n", oldMode);
      
      settings.mode = mode;
      Serial.printf("BLE: Updated mode in settings to: %d\n", settings.mode);
      
      settingsManager->saveSettings();
      Serial.printf("BLE: Mode changed via BLE: %d -> %d (SAVED)\n", oldMode, mode);
      
      // Small delay to ensure settings are written
      delay(10);
      
      // Reload settings from flash memory to update the in-memory structure
      settingsManager->loadSettings();
      
      // Verify the setting was actually saved to flash memory
      settingsManager->verifySettings();
      
      // Update the characteristic value so reads return the new value
      pModeCharacteristic->setValue(&mode, 1);
      Serial.printf("DEBUG: Updated mode characteristic value to: %d\n", mode);
      
      // Also update the status to reflect the new mode immediately
      Serial.printf("DEBUG: Mode change complete - characteristic updated, settings reloaded, mode: %d\n", mode);
    } else {
      Serial.printf("BLE: Invalid mode value received: %d (must be 0-2)\n", mode);
    }
  } else {
    Serial.printf("BLE: Invalid mode data length: %d (expected 1)\n", value.length());
  }
}

void AfterburnerBLEService::handleStartColorWrite(BLECharacteristic* pCharacteristic) {
  Serial.println("BLE: 🎨 handleStartColorWrite called!");
  String value = pCharacteristic->getValue();
  if (value.length() == 3) {
    AfterburnerSettings& settings = settingsManager->getSettings();
    uint8_t oldColor[3] = {settings.startColor[0], settings.startColor[1], settings.startColor[2]};
    settings.startColor[0] = value.charAt(0);
    settings.startColor[1] = value.charAt(1);
    settings.startColor[2] = value.charAt(2);
    settingsManager->saveSettings();
    Serial.printf("BLE: Start color changed via BLE: R%d G%d B%d -> R%d G%d B%d\n", 
                  oldColor[0], oldColor[1], oldColor[2], value.charAt(0), value.charAt(1), value.charAt(2));
    
    // Reload settings from flash memory to update the in-memory structure
    settingsManager->loadSettings();
    
    // Verify the setting was actually saved
    settingsManager->verifySettings();
  } else {
    Serial.printf("BLE: Invalid start color data length: %d\n", value.length());
  }
}

void AfterburnerBLEService::handleEndColorWrite(BLECharacteristic* pCharacteristic) {
  Serial.println("BLE: 🎨 handleEndColorWrite called!");
  String value = pCharacteristic->getValue();
  if (value.length() == 3) {
    AfterburnerSettings& settings = settingsManager->getSettings();
    uint8_t oldColor[3] = {settings.endColor[0], settings.endColor[1], settings.endColor[2]};
    settings.endColor[0] = value.charAt(0);
    settings.endColor[1] = value.charAt(1);
    settings.endColor[2] = value.charAt(2);
    settingsManager->saveSettings();
    Serial.printf("BLE: End color changed via BLE: R%d G%d B%d -> R%d G%d B%d\n", 
                  oldColor[0], oldColor[1], oldColor[2], value.charAt(0), value.charAt(1), value.charAt(2));
    
    // Reload settings from flash memory to update the in-memory structure
    settingsManager->loadSettings();
    
    // Verify the setting was actually saved
    settingsManager->verifySettings();
  } else {
    Serial.printf("BLE: Invalid end color data length: %d\n", value.length());
  }
}

void AfterburnerBLEService::handleSpeedMsWrite(BLECharacteristic* pCharacteristic) {
  Serial.println("BLE: ⚡ handleSpeedMsWrite called!");
  String value = pCharacteristic->getValue();
  if (value.length() == 2) {
    uint8_t speedBytes[2] = {value.charAt(0), value.charAt(1)};
    uint16_t speedMs = bytesToUint16(speedBytes);
    if (speedMs >= 100 && speedMs <= 5000) {
      AfterburnerSettings& settings = settingsManager->getSettings();
      uint16_t oldSpeed = settings.speedMs;
      settings.speedMs = speedMs;
      settingsManager->saveSettings();
      Serial.printf("BLE: Speed changed via BLE: %dms -> %dms\n", oldSpeed, speedMs);
      
      // Reload settings from flash memory to update the in-memory structure
      settingsManager->loadSettings();
      
      // Verify the setting was actually saved
      settingsManager->verifySettings();
    } else {
      Serial.printf("BLE: Invalid speed value received: %dms (valid range: 100-5000ms)\n", speedMs);
    }
  } else {
    Serial.printf("BLE: Invalid speed data length: %d\n", value.length());
  }
}

void AfterburnerBLEService::handleBrightnessWrite(BLECharacteristic* pCharacteristic) {
  Serial.println("BLE: 💡 handleBrightnessWrite called!");
  String value = pCharacteristic->getValue();
  if (value.length() == 1) {
    uint8_t brightness = value.charAt(0);
    if (brightness >= 10 && brightness <= 255) {
      AfterburnerSettings& settings = settingsManager->getSettings();
      uint8_t oldBrightness = settings.brightness;
      settings.brightness = brightness;
      settingsManager->saveSettings();
      Serial.printf("BLE: Brightness changed via BLE: %d -> %d\n", oldBrightness, brightness);
      
      // Reload settings from flash memory to update the in-memory structure
      settingsManager->loadSettings();
      
      // Verify the setting was actually saved
      settingsManager->verifySettings();
    } else {
      Serial.printf("BLE: Invalid brightness value received: %d (valid range: 10-255)\n", brightness);
    }
  } else {
    Serial.printf("BLE: Invalid brightness data length: %d\n", value.length());
  }
}

void AfterburnerBLEService::handleNumLedsWrite(BLECharacteristic* pCharacteristic) {
  Serial.println("BLE: 🔢 handleNumLedsWrite called!");
  String value = pCharacteristic->getValue();
  
  // Accept write for backward compatibility, but ignore value
  // New hardware has fixed 36 LEDs (4 channels × 9 LEDs), so numLeds setting is ignored
  if (value.length() == 2) {
    uint8_t numLedsBytes[2] = {value.charAt(0), value.charAt(1)};
    uint16_t numLeds = bytesToUint16(numLedsBytes);
    Serial.printf("BLE: Num LEDs write received: %d (ignored - fixed at 36 LEDs, 4 channels)\n", numLeds);
    
    // No-op: Don't update settings, don't save
    // This allows old apps to connect without errors, but the setting has no effect
    // The characteristic will always return 36 when read
  } else {
    Serial.printf("BLE: Invalid LED count data length: %d (ignored)\n", value.length());
  }
}

void AfterburnerBLEService::handleAbThresholdWrite(BLECharacteristic* pCharacteristic) {
  Serial.println("BLE: 🎯 handleAbThresholdWrite called!");
  String value = pCharacteristic->getValue();
  if (value.length() == 1) {
    uint8_t threshold = value.charAt(0);
    if (threshold <= 100) {
      AfterburnerSettings& settings = settingsManager->getSettings();
      uint8_t oldThreshold = settings.abThreshold;
      settings.abThreshold = threshold;
      settingsManager->saveSettings();
      Serial.printf("BLE: AB Threshold changed via BLE: %d%% -> %d%%\n", oldThreshold, threshold);
      
      // Reload settings from flash memory to update the in-memory structure
      settingsManager->loadSettings();
      
      // Verify the setting was actually saved
      settingsManager->verifySettings();
    } else {
      Serial.printf("BLE: Invalid AB threshold value received: %d%% (valid range: 0-100%%)\n", threshold);
    }
  } else {
    Serial.printf("BLE: Invalid AB threshold data length: %d\n", value.length());
  }
}

void AfterburnerBLEService::handleSavePresetWrite(BLECharacteristic* pCharacteristic) {
  Serial.println("BLE: 💾 handleSavePresetWrite called!");
  String value = pCharacteristic->getValue();
  Serial.printf("BLE: Save preset command received - length: %d, value: %d\n", 
                value.length(), value.length() > 0 ? value.charAt(0) : -1);
  
  if (value.length() == 1 && value.charAt(0) == 1) {
    Serial.println("BLE: Save preset command received via BLE");
    
    settingsManager->saveSettings();
    Serial.println("BLE: Settings saved to flash memory SUCCESSFULLY");
    
    // Reload settings from flash memory to update the in-memory structure
    settingsManager->loadSettings();
    
    // Verify the settings were actually saved
    settingsManager->verifySettings();
  } else {
    Serial.printf("BLE: Invalid save preset command received: length=%d, value=%d\n", 
                  value.length(), value.length() > 0 ? value.charAt(0) : -1);
  }
}

uint16_t AfterburnerBLEService::bytesToUint16(const uint8_t* data) {
  return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

void AfterburnerBLEService::uint16ToBytes(uint16_t value, uint8_t* data) {
  data[0] = value & 0xFF;
  data[1] = (value >> 8) & 0xFF;
}

// Throttle calibration handlers
void AfterburnerBLEService::handleThrottleCalibrationWrite(BLECharacteristic* pCharacteristic) {
  Serial.println("BLE: 🎯 handleThrottleCalibrationWrite called!");
  String value = pCharacteristic->getValue();
  
  if (value.length() == 1 && value.charAt(0) == 1) {
    Serial.println("BLE: 🎯 Start throttle calibration command received via BLE");
    
    // Update BLE status to show calibration is in progress
    updateThrottleCalibrationStatus(false, 0, 0);
    
    // Start calibration by setting the global flag
    startThrottleCalibration();
    
    Serial.println("BLE: 🎯 Throttle calibration started - move throttle to min and max positions");
  } else {
    Serial.printf("BLE: 🎯 Invalid throttle calibration command received: length=%d, value=%d\n", 
                  value.length(), value.length() > 0 ? value.charAt(0) : -1);
  }
}

void AfterburnerBLEService::handleThrottleCalibrationResetWrite(BLECharacteristic* pCharacteristic) {
  Serial.println("BLE: 🔄 handleThrottleCalibrationResetWrite called!");
  String value = pCharacteristic->getValue();
  
  if (value.length() == 1 && value.charAt(0) == 1) {
    Serial.println("BLE: 🔄 Reset throttle calibration command received via BLE");
    
    // Reset throttle calibration to defaults
    settingsManager->resetThrottleCalibration();
    
    // Update throttle reader with default values
    if (throttleReader) {
      throttleReader->resetCalibrationToDefaults();
    }
    
    // Update BLE calibration status characteristic with reset values
    updateThrottleCalibrationStatus(false, DEFAULT_THROTTLE_MIN, DEFAULT_THROTTLE_MAX);
    
    Serial.println("BLE: 🔄 Throttle calibration reset to defaults successfully");
  } else {
    Serial.printf("BLE: 🔄 Invalid throttle calibration reset command received: length=%d, value=%d\n", 
                  value.length(), value.length() > 0 ? value.charAt(0) : -1);
  }
}
