#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <U8g2lib.h>
#include "settings.h"

// OLED Display class for ESP32 C3
class OLEDDisplay {
private:
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display;
  unsigned long lastUpdate;
  unsigned long updateInterval;
  
  // Display state
  uint8_t currentPage;
  uint8_t totalPages;
  bool displayEnabled;
  
  // Button handling
  uint8_t navButtonPin;
  bool lastButtonState;
  unsigned long lastButtonPress;
  unsigned long buttonDebounceTime;
  
  // Mode names
  const char* modeNames[3] = {"Linear", "Ease", "Pulse"};
  
public:
  OLEDDisplay();
  void begin();
  void begin(uint8_t buttonPin); // Overloaded with button pin
  void update(const AfterburnerSettings& settings, float throttle, bool bleConnected);
  void setUpdateInterval(unsigned long interval);
  void enable(bool enabled);
  void nextPage();
  void setPage(uint8_t page);
  uint8_t getCurrentPage();
  void setNavButtonPin(uint8_t pin);
  void handleButton(); // Handle button press for navigation
  
private:
  void drawMainPage(const AfterburnerSettings& settings, float throttle, bool bleConnected);
  void drawSettingsPage(const AfterburnerSettings& settings);
  void drawStatusPage(const AfterburnerSettings& settings, float throttle, bool bleConnected);
  void drawThrottleBar(float throttle);
  void drawModeIndicator(uint8_t mode);
  void drawConnectionStatus(bool bleConnected);
  void drawColorPreview(const uint8_t* color);
  void formatThrottle(float throttle, char* buffer);
  void formatSpeed(uint16_t speedMs, char* buffer);
};

#endif // OLED_DISPLAY_H
