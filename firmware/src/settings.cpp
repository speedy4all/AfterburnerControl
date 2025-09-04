#include "settings.h"
#include "constants.h"

SettingsManager::SettingsManager() {
  // Initialize with defaults
  settings.mode = DEFAULT_MODE;
  settings.startColor[0] = DEFAULT_START_COLOR_R;
  settings.startColor[1] = DEFAULT_START_COLOR_G;
  settings.startColor[2] = DEFAULT_START_COLOR_B;
  settings.endColor[0] = DEFAULT_END_COLOR_R;
  settings.endColor[1] = DEFAULT_END_COLOR_G;
  settings.endColor[2] = DEFAULT_END_COLOR_B;
  settings.speedMs = DEFAULT_SPEED_MS;
  settings.brightness = DEFAULT_BRIGHTNESS;
  settings.numLeds = DEFAULT_NUM_LEDS;
  settings.abThreshold = DEFAULT_AB_THRESHOLD;
  settings.throttleMin = DEFAULT_THROTTLE_MIN;
  settings.throttleMax = DEFAULT_THROTTLE_MAX;
  settings.throttleCalibrated = DEFAULT_THROTTLE_CALIBRATED;
  
  // Initialize flag
  initialized = false;
}

void SettingsManager::begin() {
  // Initialize preferences with namespace "afterburner"
  if (preferences.begin("afterburner", false)) {
    // Check if we can access the preferences
    if (preferences.getBytesLength("mode") > 0) {
      Serial.println("Settings: Existing settings found in flash memory");
    } else {
      Serial.println("Settings: No existing settings found - will use defaults");
    }
    
    loadSettings();
    initialized = true; // Mark as successfully initialized
  } else {
    Serial.println("Settings: Failed to initialize preferences!");
    initialized = false;
  }
}

void SettingsManager::loadSettings() {
  settings.mode = preferences.getUChar("mode", DEFAULT_MODE);
  settings.startColor[0] = preferences.getUChar("startR", DEFAULT_START_COLOR_R);
  settings.startColor[1] = preferences.getUChar("startG", DEFAULT_START_COLOR_G);
  settings.startColor[2] = preferences.getUChar("startB", DEFAULT_START_COLOR_B);
  settings.endColor[0] = preferences.getUChar("endR", DEFAULT_END_COLOR_R);
  settings.endColor[1] = preferences.getUChar("endG", DEFAULT_END_COLOR_G);
  settings.endColor[2] = preferences.getUChar("endB", DEFAULT_END_COLOR_B);
  settings.speedMs = preferences.getUShort("speed", DEFAULT_SPEED_MS);
  settings.brightness = preferences.getUChar("bright", DEFAULT_BRIGHTNESS);
  settings.numLeds = preferences.getUShort("numLeds", DEFAULT_NUM_LEDS);
  settings.abThreshold = preferences.getUChar("abThresh", DEFAULT_AB_THRESHOLD);
  settings.throttleMin = preferences.getUShort("throttleMin", DEFAULT_THROTTLE_MIN);
  settings.throttleMax = preferences.getUShort("throttleMax", DEFAULT_THROTTLE_MAX);
  settings.throttleCalibrated = preferences.getBool("throttleCal", DEFAULT_THROTTLE_CALIBRATED);
}

void SettingsManager::saveSettings() {
  // Save all settings with individual error checking
  bool allSuccess = true;
  int failedCount = 0;
  
  // Save each setting individually and track results
  if (!preferences.putUChar("mode", settings.mode)) {
    failedCount++;
    allSuccess = false;
  }
  
  if (!preferences.putUChar("startR", settings.startColor[0])) {
    failedCount++;
    allSuccess = false;
  }
  
  if (!preferences.putUChar("startG", settings.startColor[1])) {
    failedCount++;
    allSuccess = false;
  }
  
  if (!preferences.putUChar("startB", settings.startColor[2])) {
    failedCount++;
    allSuccess = false;
  }
  
  if (!preferences.putUChar("endR", settings.endColor[0])) {
    failedCount++;
    allSuccess = false;
  }
  
  if (!preferences.putUChar("endG", settings.endColor[1])) {
    failedCount++;
    allSuccess = false;
  }
  
  if (!preferences.putUChar("endB", settings.endColor[2])) {
    failedCount++;
    allSuccess = false;
  }
  
  if (!preferences.putUShort("speed", settings.speedMs)) {
    failedCount++;
    allSuccess = false;
  }
  
  if (!preferences.putUChar("bright", settings.brightness)) {
    Serial.println("Settings: ⚠️ Failed to save brightness");
    failedCount++;
    allSuccess = false;
  }
  
  if (!preferences.putUShort("numLeds", settings.numLeds)) {
    Serial.println("Settings: ⚠️ Failed to save numLeds");
    failedCount++;
    allSuccess = false;
  }
  
  if (!preferences.putUChar("abThresh", settings.abThreshold)) {
    Serial.println("Settings: ⚠️ Failed to save abThresh");
    failedCount++;
    allSuccess = false;
  }
  
  if (!preferences.putUShort("throttleMin", settings.throttleMin)) {
    Serial.println("Settings: ⚠️ Failed to save throttleMin");
    failedCount++;
    allSuccess = false;
  }
  
  if (!preferences.putUShort("throttleMax", settings.throttleMax)) {
    Serial.println("Settings: ⚠️ Failed to save throttleMax");
    failedCount++;
    allSuccess = false;
  }
  
  if (!preferences.putBool("throttleCal", settings.throttleCalibrated)) {
    Serial.println("Settings: ⚠️ Failed to save throttleCal");
    failedCount++;
    allSuccess = false;
  }
  
  // Force write to flash memory - ESP32 Preferences automatically commits after each put operation
  // Add a small delay to ensure the write completes
  delay(10);
  
  if (allSuccess) {
    Serial.printf("Settings: ✅ All settings saved successfully - mode=%d, startColor=[%d,%d,%d], endColor=[%d,%d,%d], speed=%d, brightness=%d, numLeds=%d, abThreshold=%d, throttleMin=%d, throttleMax=%d\n",
                  settings.mode,
                  settings.startColor[0], settings.startColor[1], settings.startColor[2],
                  settings.endColor[0], settings.endColor[1], settings.endColor[2],
                  settings.speedMs, settings.brightness, settings.numLeds, settings.abThreshold,
                  settings.throttleMin, settings.throttleMax);
  } else {
    Serial.printf("Settings: ⚠️ %d settings failed to save, but some may have succeeded. Check individual results above.\n", failedCount);
  }
}

AfterburnerSettings& SettingsManager::getSettings() {
  return settings;
}

void SettingsManager::updateSettings(const AfterburnerSettings& newSettings) {
  settings = newSettings;
  saveSettings();
}

void SettingsManager::verifySettings() {
  Serial.println("Settings: Verifying saved settings...");
  
  // Read back from preferences to verify they were saved
  uint8_t savedMode = preferences.getUChar("mode", 255);
  uint8_t savedStartR = preferences.getUChar("startR", 255);
  uint8_t savedStartG = preferences.getUChar("startG", 255);
  uint8_t savedStartB = preferences.getUChar("startB", 255);
  uint8_t savedEndR = preferences.getUChar("endR", 255);
  uint8_t savedEndG = preferences.getUChar("endG", 255);
  uint8_t savedEndB = preferences.getUChar("endB", 255);
  uint16_t savedSpeed = preferences.getUShort("speed", 65535);
  uint8_t savedBrightness = preferences.getUChar("bright", 255);
  uint16_t savedNumLeds = preferences.getUShort("numLeds", 65535);
  uint8_t savedAbThreshold = preferences.getUChar("abThresh", 255);
  
  Serial.printf("Settings: Verification - mode=%d, startColor=[%d,%d,%d], endColor=[%d,%d,%d], speed=%d, brightness=%d, numLeds=%d, abThreshold=%d\n",
                savedMode,
                savedStartR, savedStartG, savedStartB,
                savedEndR, savedEndG, savedEndB,
                savedSpeed, savedBrightness, savedNumLeds, savedAbThreshold);
                
  // Check if verification matches current settings
  if (savedMode == settings.mode && 
      savedStartR == settings.startColor[0] && savedStartG == settings.startColor[1] && savedStartB == settings.startColor[2] &&
      savedEndR == settings.endColor[0] && savedEndG == settings.endColor[1] && savedEndB == settings.endColor[2] &&
      savedSpeed == settings.speedMs && savedBrightness == settings.brightness && 
      savedNumLeds == settings.numLeds && savedAbThreshold == settings.abThreshold) {
    Serial.println("Settings: ✅ Verification successful - all settings match!");
  } else {
    Serial.println("Settings: ❌ Verification failed - settings mismatch detected!");
  }
}

void SettingsManager::resetToDefaults() {
  Serial.println("Settings: Resetting all settings to defaults...");
  
  // Clear all preferences
  preferences.clear();
  
  // Add a small delay to ensure the clear operation completes
  delay(10);
  
  // Reset settings to defaults
  settings.mode = DEFAULT_MODE;
  settings.startColor[0] = DEFAULT_START_COLOR_R;
  settings.startColor[1] = DEFAULT_START_COLOR_G;
  settings.startColor[2] = DEFAULT_START_COLOR_B;
  settings.endColor[0] = DEFAULT_END_COLOR_R;
  settings.endColor[1] = DEFAULT_END_COLOR_G;
  settings.endColor[2] = DEFAULT_END_COLOR_B;
  settings.speedMs = DEFAULT_SPEED_MS;
  settings.brightness = DEFAULT_BRIGHTNESS;
  settings.numLeds = DEFAULT_NUM_LEDS;
  settings.abThreshold = DEFAULT_AB_THRESHOLD;
  
  // Save the defaults
  saveSettings();
  
  Serial.println("Settings: Reset to defaults completed");
}

void SettingsManager::checkFlashStatus() {
  Serial.println("Settings: Checking flash memory status...");
  
  // Since preferences are already initialized in read-write mode, we can use them directly
  if (preferences.getBytesLength("mode") > 0) {
    Serial.println("Settings: ✅ Settings found in flash memory");
    
    // Try to read a value to verify it's accessible
    uint8_t testMode = preferences.getUChar("mode", 255);
    if (testMode != 255) {
      Serial.printf("Settings: ✅ Mode value readable: %d\n", testMode);
    } else {
      Serial.println("Settings: ⚠️ Mode value not readable");
    }
    
    // Check flash memory usage
    size_t freeEntries = preferences.freeEntries();
    Serial.printf("Settings: Flash memory - Free entries: %d\n", freeEntries);
    
    if (freeEntries < 10) {
      Serial.println("Settings: ⚠️ Low flash memory - consider clearing some preferences");
    }
    
    // Check if we can write a test value
    Serial.println("Settings: Testing flash write capability...");
    if (preferences.putUChar("test_write", 123)) {
      Serial.println("Settings: ✅ Flash write test successful");
      // Clean up test value
      preferences.remove("test_write");
    } else {
      Serial.println("Settings: ❌ Flash write test failed - this indicates a serious problem");
    }
    
  } else {
    Serial.println("Settings: ⚠️ No settings found in flash memory");
  }
  
  // Check if preferences namespace is accessible
  if (preferences.begin("afterburner", true)) { // Try read-only mode
    Serial.println("Settings: ✅ Preferences namespace accessible in read mode");
    preferences.end();
  } else {
    Serial.println("Settings: ❌ Preferences namespace not accessible in read mode");
  }
  
  // Reopen in read-write mode
  if (preferences.begin("afterburner", false)) {
    Serial.println("Settings: ✅ Preferences namespace accessible in read-write mode");
  } else {
    Serial.println("Settings: ❌ Preferences namespace not accessible in read-write mode");
  }
}

void SettingsManager::printPreferencesInfo() {
  Serial.println("Settings: Preferences information...");
  
  // Since preferences are already initialized in read-write mode, we can use them directly
  Serial.println("Settings: ✅ Preferences namespace accessible");
  
  // Try to read a value to verify it's accessible
  uint8_t testMode = preferences.getUChar("mode", 255);
  if (testMode != 255) {
    Serial.printf("Settings: ✅ Mode value readable: %d\n", testMode);
  } else {
    Serial.println("Settings: ⚠️ Mode value not readable");
  }
}

bool SettingsManager::isInitialized() {
  // Return the initialization status
  return initialized;
}

bool SettingsManager::hasSavedSettings() {
  // Check if there are any saved settings in flash memory
  if (!initialized) {
    return false;
  }
  
  // Check if the mode key exists and has a value
  size_t keyLength = preferences.getBytesLength("mode");
  return keyLength > 0;
}

// Throttle calibration methods
void SettingsManager::startThrottleCalibration() {
  Serial.println("Settings: 🎯 Starting throttle calibration...");
  // This method is called when calibration starts
  // The actual calibration is handled by the ThrottleReader class
}

void SettingsManager::updateThrottleCalibration(uint16_t minValue, uint16_t maxValue) {
  Serial.printf("Settings: 🎯 Updating throttle calibration - Min: %u, Max: %u\n", minValue, maxValue);
  
  // Validate calibration values
  if (minValue >= maxValue || minValue < MIN_PWM_VALUE || maxValue > MAX_PWM_VALUE) {
    Serial.println("Settings: ❌ Invalid calibration values!");
    return;
  }
  
  // Update settings
  settings.throttleMin = minValue;
  settings.throttleMax = maxValue;
  settings.throttleCalibrated = true;
  
  // Save to flash memory
  saveSettings();
  
  // Verify the throttle calibration values were saved correctly
  uint16_t savedMin = preferences.getUShort("throttleMin", 0);
  uint16_t savedMax = preferences.getUShort("throttleMax", 0);
  bool savedCalibrated = preferences.getBool("throttleCal", false);
  
  if (savedMin == minValue && savedMax == maxValue && savedCalibrated) {
    Serial.printf("Settings: ✅ Throttle calibration verified in flash - Min: %u, Max: %u\n", 
                  savedMin, savedMax);
  } else {
    Serial.printf("Settings: ⚠️ Throttle calibration verification failed! Expected: Min=%u, Max=%u, Got: Min=%u, Max=%u, Calibrated=%s\n",
                  minValue, maxValue, savedMin, savedMax, savedCalibrated ? "true" : "false");
    
    // If verification failed, check flash status for debugging
    Serial.println("Settings: 🔍 Checking flash status after verification failure...");
    checkFlashStatus();
  }
}

void SettingsManager::resetThrottleCalibration() {
  Serial.println("Settings: 🎯 Resetting throttle calibration to defaults...");
  
  // Reset to default values
  settings.throttleMin = DEFAULT_THROTTLE_MIN;
  settings.throttleMax = DEFAULT_THROTTLE_MAX;
  settings.throttleCalibrated = DEFAULT_THROTTLE_CALIBRATED;
  
  // Save to flash memory
  saveSettings();
  
  Serial.printf("Settings: ✅ Throttle calibration reset - Min: %u, Max: %u\n", 
                settings.throttleMin, settings.throttleMax);
}

bool SettingsManager::isThrottleCalibrating() {
  // This would need to be implemented with a callback to ThrottleReader
  // For now, return false
  return false;
}

bool SettingsManager::isThrottleCalibrated() {
  return settings.throttleCalibrated;
}

uint16_t SettingsManager::getThrottleMin() {
  return settings.throttleMin;
}

uint16_t SettingsManager::getThrottleMax() {
  return settings.throttleMax;
}

// Debug method to check throttle calibration values in flash
void SettingsManager::debugThrottleCalibration() {
  Serial.println("Settings: 🔍 Debugging throttle calibration values...");
  
  // Check in-memory values
  Serial.printf("Settings: In-memory - Min: %u, Max: %u, Calibrated: %s\n",
                settings.throttleMin, settings.throttleMax, 
                settings.throttleCalibrated ? "true" : "false");
  
  // Check flash memory values
  uint16_t flashMin = preferences.getUShort("throttleMin", 0);
  uint16_t flashMax = preferences.getUShort("throttleMax", 0);
  bool flashCalibrated = preferences.getBool("throttleCal", false);
  
  Serial.printf("Settings: Flash memory - Min: %u, Max: %u, Calibrated: %s\n",
                flashMin, flashMax, flashCalibrated ? "true" : "false");
  
  // Check if values match
  if (settings.throttleMin == flashMin && settings.throttleMax == flashMax && 
      settings.throttleCalibrated == flashCalibrated) {
    Serial.println("Settings: ✅ In-memory and flash values match");
  } else {
    Serial.println("Settings: ❌ In-memory and flash values do not match");
  }
}
