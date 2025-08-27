#include "ble_service.h"
#include <ArduinoJson.h>

// Callback classes for BLE characteristics
class ModeCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  ModeCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    bleService->handleModeWrite(pCharacteristic);
  }
};

class StartColorCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  StartColorCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    bleService->handleStartColorWrite(pCharacteristic);
  }
};

class EndColorCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  EndColorCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    bleService->handleEndColorWrite(pCharacteristic);
  }
};

class SpeedMsCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  SpeedMsCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    bleService->handleSpeedMsWrite(pCharacteristic);
  }
};

class BrightnessCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  BrightnessCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    bleService->handleBrightnessWrite(pCharacteristic);
  }
};

class NumLedsCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  NumLedsCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    bleService->handleNumLedsWrite(pCharacteristic);
  }
};

class AbThresholdCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  AbThresholdCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    bleService->handleAbThresholdWrite(pCharacteristic);
  }
};

class SavePresetCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
private:
  AfterburnerBLEService* bleService;
public:
  SavePresetCharacteristicCallbacks(AfterburnerBLEService* service) : bleService(service) {}
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    bleService->handleSavePresetWrite(pCharacteristic);
  }
};

AfterburnerBLEService::AfterburnerBLEService(SettingsManager* settings) {
  settingsManager = settings;
  pServer = nullptr;
  pService = nullptr;
  lastStatusUpdate = 0;
  
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
}

void AfterburnerBLEService::begin() {
  // Initialize BLE device
  NimBLEDevice::init(DEVICE_NAME);
  NimBLEDevice::setPower(ESP_PWR_LVL_P7); // Medium power
  
  // Create BLE server
  pServer = NimBLEDevice::createServer();
  
  // Create service
  createService();
  
  // Start advertising
  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  NimBLEDevice::startAdvertising();
  
  Serial.println("BLE service started");
}

void AfterburnerBLEService::createService() {
  pService = pServer->createService(SERVICE_UUID);
  
  // Create characteristics
  pModeCharacteristic = pService->createCharacteristic(
    MODE_UUID,
    NIMBLE_PROPERTY::READ |
    NIMBLE_PROPERTY::WRITE
  );
  
  pStartColorCharacteristic = pService->createCharacteristic(
    START_COLOR_UUID,
    NIMBLE_PROPERTY::READ |
    NIMBLE_PROPERTY::WRITE
  );
  
  pEndColorCharacteristic = pService->createCharacteristic(
    END_COLOR_UUID,
    NIMBLE_PROPERTY::READ |
    NIMBLE_PROPERTY::WRITE
  );
  
  pSpeedMsCharacteristic = pService->createCharacteristic(
    SPEED_MS_UUID,
    NIMBLE_PROPERTY::READ |
    NIMBLE_PROPERTY::WRITE
  );
  
  pBrightnessCharacteristic = pService->createCharacteristic(
    BRIGHTNESS_UUID,
    NIMBLE_PROPERTY::READ |
    NIMBLE_PROPERTY::WRITE
  );
  
  pNumLedsCharacteristic = pService->createCharacteristic(
    NUM_LEDS_UUID,
    NIMBLE_PROPERTY::READ |
    NIMBLE_PROPERTY::WRITE
  );
  
  pAbThresholdCharacteristic = pService->createCharacteristic(
    AB_THRESHOLD_UUID,
    NIMBLE_PROPERTY::READ |
    NIMBLE_PROPERTY::WRITE
  );
  
  pSavePresetCharacteristic = pService->createCharacteristic(
    SAVE_PRESET_UUID,
    NIMBLE_PROPERTY::WRITE
  );
  
  pStatusCharacteristic = pService->createCharacteristic(
    STATUS_UUID,
    NIMBLE_PROPERTY::READ |
    NIMBLE_PROPERTY::NOTIFY
  );
  
  // Start the service
  pService->start();
  
  // Setup callbacks
  setupCallbacks();
  
  // Set initial values
  updateCharacteristicValues();
}

void AfterburnerBLEService::setupCallbacks() {
  pModeCharacteristic->setCallbacks(new ModeCharacteristicCallbacks(this));
  pStartColorCharacteristic->setCallbacks(new StartColorCharacteristicCallbacks(this));
  pEndColorCharacteristic->setCallbacks(new EndColorCharacteristicCallbacks(this));
  pSpeedMsCharacteristic->setCallbacks(new SpeedMsCharacteristicCallbacks(this));
  pBrightnessCharacteristic->setCallbacks(new BrightnessCharacteristicCallbacks(this));
  pNumLedsCharacteristic->setCallbacks(new NumLedsCharacteristicCallbacks(this));
  pAbThresholdCharacteristic->setCallbacks(new AbThresholdCharacteristicCallbacks(this));
  pSavePresetCharacteristic->setCallbacks(new SavePresetCharacteristicCallbacks(this));
}

void AfterburnerBLEService::updateCharacteristicValues() {
  AfterburnerSettings& settings = settingsManager->getSettings();
  
  // Set initial values
  pModeCharacteristic->setValue(&settings.mode, 1);
  pStartColorCharacteristic->setValue(settings.startColor, 3);
  pEndColorCharacteristic->setValue(settings.endColor, 3);
  
  uint8_t speedBytes[2];
  uint16ToBytes(settings.speedMs, speedBytes);
  pSpeedMsCharacteristic->setValue(speedBytes, 2);
  
  pBrightnessCharacteristic->setValue(&settings.brightness, 1);
  
  uint8_t numLedsBytes[2];
  uint16ToBytes(settings.numLeds, numLedsBytes);
  pNumLedsCharacteristic->setValue(numLedsBytes, 2);
  
  pAbThresholdCharacteristic->setValue(&settings.abThreshold, 1);
}

void AfterburnerBLEService::updateStatus(float throttle, uint8_t mode) {
  // Send status notification every 200ms
  if (millis() - lastStatusUpdate > 200) {
    // Create JSON status
    StaticJsonDocument<64> doc;
    doc["thr"] = round(throttle * 100) / 100.0; // Round to 2 decimal places
    doc["mode"] = mode;
    
    String statusJson;
    serializeJson(doc, statusJson);
    
    pStatusCharacteristic->setValue(statusJson.c_str());
    pStatusCharacteristic->notify();
    
    lastStatusUpdate = millis();
  }
}

bool AfterburnerBLEService::isConnected() {
  if (pServer) {
    return pServer->getConnectedCount() > 0;
  }
  return false;
}

void AfterburnerBLEService::handleModeWrite(NimBLECharacteristic* pCharacteristic) {
  std::string value = pCharacteristic->getValue();
  if (value.length() == 1) {
    uint8_t mode = value[0];
    if (mode <= 2) {
      AfterburnerSettings& settings = settingsManager->getSettings();
      settings.mode = mode;
      settingsManager->saveSettings();
    }
  }
}

void AfterburnerBLEService::handleStartColorWrite(NimBLECharacteristic* pCharacteristic) {
  std::string value = pCharacteristic->getValue();
  if (value.length() == 3) {
    AfterburnerSettings& settings = settingsManager->getSettings();
    settings.startColor[0] = value[0];
    settings.startColor[1] = value[1];
    settings.startColor[2] = value[2];
    settingsManager->saveSettings();
  }
}

void AfterburnerBLEService::handleEndColorWrite(NimBLECharacteristic* pCharacteristic) {
  std::string value = pCharacteristic->getValue();
  if (value.length() == 3) {
    AfterburnerSettings& settings = settingsManager->getSettings();
    settings.endColor[0] = value[0];
    settings.endColor[1] = value[1];
    settings.endColor[2] = value[2];
    settingsManager->saveSettings();
  }
}

void AfterburnerBLEService::handleSpeedMsWrite(NimBLECharacteristic* pCharacteristic) {
  std::string value = pCharacteristic->getValue();
  if (value.length() == 2) {
    uint16_t speedMs = bytesToUint16((uint8_t*)value.c_str());
    if (speedMs >= 100 && speedMs <= 5000) {
      AfterburnerSettings& settings = settingsManager->getSettings();
      settings.speedMs = speedMs;
      settingsManager->saveSettings();
    }
  }
}

void AfterburnerBLEService::handleBrightnessWrite(NimBLECharacteristic* pCharacteristic) {
  std::string value = pCharacteristic->getValue();
  if (value.length() == 1) {
    uint8_t brightness = value[0];
    if (brightness >= 10 && brightness <= 255) {
      AfterburnerSettings& settings = settingsManager->getSettings();
      settings.brightness = brightness;
      settingsManager->saveSettings();
    }
  }
}

void AfterburnerBLEService::handleNumLedsWrite(NimBLECharacteristic* pCharacteristic) {
  std::string value = pCharacteristic->getValue();
  if (value.length() == 2) {
    uint16_t numLeds = bytesToUint16((uint8_t*)value.c_str());
    if (numLeds >= 1 && numLeds <= 300) {
      AfterburnerSettings& settings = settingsManager->getSettings();
      settings.numLeds = numLeds;
      settingsManager->saveSettings();
    }
  }
}

void AfterburnerBLEService::handleAbThresholdWrite(NimBLECharacteristic* pCharacteristic) {
  std::string value = pCharacteristic->getValue();
  if (value.length() == 1) {
    uint8_t threshold = value[0];
    if (threshold <= 100) {
      AfterburnerSettings& settings = settingsManager->getSettings();
      settings.abThreshold = threshold;
      settingsManager->saveSettings();
    }
  }
}

void AfterburnerBLEService::handleSavePresetWrite(NimBLECharacteristic* pCharacteristic) {
  std::string value = pCharacteristic->getValue();
  if (value.length() == 1 && value[0] == 1) {
    settingsManager->saveSettings();
  }
}

uint16_t AfterburnerBLEService::bytesToUint16(const uint8_t* data) {
  return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

void AfterburnerBLEService::uint16ToBytes(uint16_t value, uint8_t* data) {
  data[0] = value & 0xFF;
  data[1] = (value >> 8) & 0xFF;
}
