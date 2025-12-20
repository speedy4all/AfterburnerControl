#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <Preferences.h>
#include <Arduino.h>

// Afterburner settings structure
struct AfterburnerSettings {
  uint8_t mode;           // 0=Linear, 1=Ease, 2=Pulse
  uint8_t startColor[3];  // RGB start color (used to calculate base intensity)
  uint8_t endColor[3];    // RGB end color (used to calculate afterburner intensity)
  uint16_t speedMs;       // Animation speed in milliseconds
  uint8_t brightness;     // Brightness cap (10-255)
  uint16_t numLeds;       // Number of LEDs (DEPRECATED: kept for backward compatibility)
                          // New hardware: Fixed at 36 LEDs (4 channels × 9 LEDs)
                          // Legacy hardware: Configurable (1-300)
  uint8_t abThreshold;    // Afterburner threshold (0-100%)
  uint16_t throttleMin;   // Calibrated min throttle PWM value
  uint16_t throttleMax;   // Calibrated max throttle PWM value
  bool throttleCalibrated; // Whether throttle has been calibrated
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
#define DEFAULT_THROTTLE_MIN 900
#define DEFAULT_THROTTLE_MAX 2000
#define DEFAULT_THROTTLE_CALIBRATED false

class SettingsManager {
private:
  Preferences preferences;
  AfterburnerSettings settings;
  bool initialized;

public:
  SettingsManager();
  void begin();
  void loadSettings();
  void saveSettings();
  AfterburnerSettings& getSettings();
  void updateSettings(const AfterburnerSettings& newSettings);
  void verifySettings();
  void resetToDefaults();
  void checkFlashStatus();
  void printPreferencesInfo();
  bool isInitialized();
  bool hasSavedSettings();
  
  // Throttle calibration methods
  void startThrottleCalibration();
  void updateThrottleCalibration(uint16_t minValue, uint16_t maxValue);
  void resetThrottleCalibration();
  bool isThrottleCalibrating();
  bool isThrottleCalibrated();
  uint16_t getThrottleMin();
  uint16_t getThrottleMax();
  
  // Debug methods
  void debugThrottleCalibration();
};

#endif // SETTINGS_H
