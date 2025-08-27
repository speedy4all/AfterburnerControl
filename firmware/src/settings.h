#ifndef SETTINGS_H
#define SETTINGS_H

#include <Preferences.h>
#include <Arduino.h>

// Afterburner settings structure
struct AfterburnerSettings {
  uint8_t mode;           // 0=Linear, 1=Ease, 2=Pulse
  uint8_t startColor[3];  // RGB start color
  uint8_t endColor[3];    // RGB end color
  uint16_t speedMs;       // Animation speed in milliseconds
  uint8_t brightness;     // Brightness cap (10-255)
  uint16_t numLeds;       // Number of LEDs
  uint8_t abThreshold;    // Afterburner threshold (0-100%)
};

// Default settings
#define DEFAULT_MODE 1
#define DEFAULT_START_COLOR_R 255
#define DEFAULT_START_COLOR_G 100
#define DEFAULT_START_COLOR_B 0
#define DEFAULT_END_COLOR_R 154
#define DEFAULT_END_COLOR_G 0
#define DEFAULT_END_COLOR_B 255
#define DEFAULT_SPEED_MS 1200
#define DEFAULT_BRIGHTNESS 200
#define DEFAULT_NUM_LEDS 45
#define DEFAULT_AB_THRESHOLD 80

class SettingsManager {
private:
  Preferences preferences;
  AfterburnerSettings settings;

public:
  SettingsManager();
  void begin();
  void loadSettings();
  void saveSettings();
  AfterburnerSettings& getSettings();
  void updateSettings(const AfterburnerSettings& newSettings);
};

#endif // SETTINGS_H
