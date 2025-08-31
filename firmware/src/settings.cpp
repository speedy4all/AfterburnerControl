#include "settings.h"

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
  settingsLoaded = false;
}

void SettingsManager::begin() {
  EEPROM.begin(512); // Initialize EEPROM with 512 bytes
  loadSettings();
}

void SettingsManager::loadSettings() {
  if (settingsLoaded) return;
  
  // Check if settings are valid (magic number)
  uint32_t magic = 0;
  for (int i = 0; i < 4; i++) {
    magic |= ((uint32_t)EEPROM.read(i) << (i * 8));
  }
  if (magic == 0xAB123456) {
    // Settings are valid, load them
    int addr = 4;
    settings.mode = EEPROM.read(addr++);
    settings.startColor[0] = EEPROM.read(addr++);
    settings.startColor[1] = EEPROM.read(addr++);
    settings.startColor[2] = EEPROM.read(addr++);
    settings.endColor[0] = EEPROM.read(addr++);
    settings.endColor[1] = EEPROM.read(addr++);
    settings.endColor[2] = EEPROM.read(addr++);
    settings.speedMs = (EEPROM.read(addr) | (EEPROM.read(addr + 1) << 8)); addr += 2;
    settings.brightness = EEPROM.read(addr++);
    settings.numLeds = (EEPROM.read(addr) | (EEPROM.read(addr + 1) << 8)); addr += 2;
    settings.abThreshold = EEPROM.read(addr++);
    Serial.println("Settings loaded from EEPROM");
  } else {
    // No valid settings, use defaults
    Serial.println("No valid settings found, using defaults");
    saveSettings(); // Save defaults to EEPROM
  }
  settingsLoaded = true;
}

void SettingsManager::saveSettings() {
  // Feed watchdog before starting EEPROM operations
  ESP.wdtFeed();
  
  // Write magic number
  uint32_t magic = 0xAB123456;
  for (int i = 0; i < 4; i++) {
    EEPROM.write(i, (magic >> (i * 8)) & 0xFF);
  }
  
  // Write settings in one batch
  int addr = 4;
  EEPROM.write(addr++, settings.mode);
  EEPROM.write(addr++, settings.startColor[0]);
  EEPROM.write(addr++, settings.startColor[1]);
  EEPROM.write(addr++, settings.startColor[2]);
  EEPROM.write(addr++, settings.endColor[0]);
  EEPROM.write(addr++, settings.endColor[1]);
  EEPROM.write(addr++, settings.endColor[2]);
  EEPROM.write(addr++, settings.speedMs & 0xFF);
  EEPROM.write(addr++, (settings.speedMs >> 8) & 0xFF);
  EEPROM.write(addr++, settings.brightness);
  EEPROM.write(addr++, settings.numLeds & 0xFF);
  EEPROM.write(addr++, (settings.numLeds >> 8) & 0xFF);
  EEPROM.write(addr++, settings.abThreshold);
  
  // Feed watchdog before commit
  ESP.wdtFeed();
  
  // Commit to EEPROM
  EEPROM.commit();
  
  // Feed watchdog after commit
  ESP.wdtFeed();
  
  Serial.println("Settings saved to EEPROM");
}

AfterburnerSettings& SettingsManager::getSettings() {
  return settings;
}

void SettingsManager::updateSettings(const AfterburnerSettings& newSettings) {
  settings = newSettings;
  // Don't save immediately - let the deferred save handle it
  // saveSettings(); // Removed to prevent reboots during WebSocket events
}
