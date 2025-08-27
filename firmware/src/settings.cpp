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
}

void SettingsManager::begin() {
  preferences.begin("afterburner", false);
  loadSettings();
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
}

void SettingsManager::saveSettings() {
  preferences.putUChar("mode", settings.mode);
  preferences.putUChar("startR", settings.startColor[0]);
  preferences.putUChar("startG", settings.startColor[1]);
  preferences.putUChar("startB", settings.startColor[2]);
  preferences.putUChar("endR", settings.endColor[0]);
  preferences.putUChar("endG", settings.endColor[1]);
  preferences.putUChar("endB", settings.endColor[2]);
  preferences.putUShort("speed", settings.speedMs);
  preferences.putUChar("bright", settings.brightness);
  preferences.putUShort("numLeds", settings.numLeds);
  preferences.putUChar("abThresh", settings.abThreshold);
}

AfterburnerSettings& SettingsManager::getSettings() {
  return settings;
}

void SettingsManager::updateSettings(const AfterburnerSettings& newSettings) {
  settings = newSettings;
  saveSettings();
}
