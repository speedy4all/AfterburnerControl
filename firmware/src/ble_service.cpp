#include "ble_service.h"
#include <ArduinoJson.h>

BLEService::BLEService(SettingsManager* settings) {
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

void BLEService::begin() {
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

void BLEService::createService() {
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
  
  // Set initial values
  AfterburnerSettings& settings = settingsManager->getSettings();
  
  uint8_t modeValue = settings.mode;
  pModeCharacteristic->setValue(&modeValue, 1);
  
  pStartColorCharacteristic->setValue(settings.startColor, 3);
  pEndColorCharacteristic->setValue(settings.endColor, 3);
  
  uint8_t speedBytes[2];
  uint16ToBytes(settings.speedMs, speedBytes);
  pSpeedMsCharacteristic->setValue(speedBytes, 2);
  
  uint8_t brightnessValue = settings.brightness;
  pBrightnessCharacteristic->setValue(&brightnessValue, 1);
  
  uint8_t numLedsBytes[2];
  uint16ToBytes(settings.numLeds, numLedsBytes);
  pNumLedsCharacteristic->setValue(numLedsBytes, 2);
  
  uint8_t abThresholdValue = settings.abThreshold;
  pAbThresholdCharacteristic->setValue(&abThresholdValue, 1);
  
  // Setup callbacks
  setupCallbacks();
  
  // Start service
  pService->start();
}

void BLEService::setupCallbacks() {
  pModeCharacteristic->setCallbacks(new class : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
      handleModeWrite(pCharacteristic);
    }
  });
  
  pStartColorCharacteristic->setCallbacks(new class : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
      handleStartColorWrite(pCharacteristic);
    }
  });
  
  pEndColorCharacteristic->setCallbacks(new class : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
      handleEndColorWrite(pCharacteristic);
    }
  });
  
  pSpeedMsCharacteristic->setCallbacks(new class : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
      handleSpeedMsWrite(pCharacteristic);
    }
  });
  
  pBrightnessCharacteristic->setCallbacks(new class : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
      handleBrightnessWrite(pCharacteristic);
    }
  });
  
  pNumLedsCharacteristic->setCallbacks(new class : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
      handleNumLedsWrite(pCharacteristic);
    }
  });
  
  pAbThresholdCharacteristic->setCallbacks(new class : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
      handleAbThresholdWrite(pCharacteristic);
    }
  });
  
  pSavePresetCharacteristic->setCallbacks(new class : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
      handleSavePresetWrite(pCharacteristic);
    }
  });
}

void BLEService::updateStatus(float throttle, uint8_t mode) {
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

bool BLEService::isConnected() {
  if (pServer) {
    return pServer->getConnectedCount() > 0;
  }
  return false;
}

void BLEService::handleModeWrite(NimBLECharacteristic* pCharacteristic) {
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

void BLEService::handleStartColorWrite(NimBLECharacteristic* pCharacteristic) {
  std::string value = pCharacteristic->getValue();
  if (value.length() == 3) {
    AfterburnerSettings& settings = settingsManager->getSettings();
    settings.startColor[0] = value[0];
    settings.startColor[1] = value[1];
    settings.startColor[2] = value[2];
    settingsManager->saveSettings();
  }
}

void BLEService::handleEndColorWrite(NimBLECharacteristic* pCharacteristic) {
  std::string value = pCharacteristic->getValue();
  if (value.length() == 3) {
    AfterburnerSettings& settings = settingsManager->getSettings();
    settings.endColor[0] = value[0];
    settings.endColor[1] = value[1];
    settings.endColor[2] = value[2];
    settingsManager->saveSettings();
  }
}

void BLEService::handleSpeedMsWrite(NimBLECharacteristic* pCharacteristic) {
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

void BLEService::handleBrightnessWrite(NimBLECharacteristic* pCharacteristic) {
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

void BLEService::handleNumLedsWrite(NimBLECharacteristic* pCharacteristic) {
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

void BLEService::handleAbThresholdWrite(NimBLECharacteristic* pCharacteristic) {
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

void BLEService::handleSavePresetWrite(NimBLECharacteristic* pCharacteristic) {
  std::string value = pCharacteristic->getValue();
  if (value.length() == 1 && value[0] == 1) {
    settingsManager->saveSettings();
  }
}

uint16_t BLEService::bytesToUint16(const uint8_t* data) {
  return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

void BLEService::uint16ToBytes(uint16_t value, uint8_t* data) {
  data[0] = value & 0xFF;
  data[1] = (value >> 8) & 0xFF;
}
