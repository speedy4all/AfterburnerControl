#include "oled_display.h"
#include <Wire.h>

OLEDDisplay::OLEDDisplay() {
  // Initialize display with I2C pins for ESP32 C3
  // SDA: GPIO4, SCL: GPIO5 (default I2C pins)
  // This is a custom 128x64 OLED with starting point at (13, 14)
  display = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);
  lastUpdate = 0;
  updateInterval = 500; // Update every 500ms
  currentPage = 0;
  totalPages = 3;
  displayEnabled = true;
  
  // Button handling initialization
  navButtonPin = 0; // Default to GPIO0 (BOOT button)
  lastButtonState = HIGH;
  lastButtonPress = 0;
  buttonDebounceTime = 200; // 200ms debounce
}

void OLEDDisplay::begin() {
  begin(0); // Default to GPIO0 if no pin specified
}

void OLEDDisplay::begin(uint8_t buttonPin) {
  Wire.begin(4, 5); // SDA=4, SCL=5 for ESP32 C3
  display->begin();
  display->setFont(u8g2_font_6x10_tr);
  display->setFontDirection(0);
  display->clearBuffer();
  display->sendBuffer();
  
  // Initialize navigation button
  setNavButtonPin(buttonPin);
  
  // Show startup message - adjusted for 128x64 screen with offset
  display->clearBuffer();
  display->drawStr(15, 26, "ESP32 C3");
  display->drawStr(15, 38, "Afterburner");
  display->drawStr(15, 50, "Starting...");
  display->sendBuffer();
  delay(1000);
}

void OLEDDisplay::setNavButtonPin(uint8_t pin) {
  navButtonPin = pin;
  pinMode(navButtonPin, INPUT_PULLUP);
  lastButtonState = digitalRead(navButtonPin);
  Serial.printf("Navigation button initialized on GPIO%d\n", pin);
}

void OLEDDisplay::handleButton() {
  if (navButtonPin == 0) return; // No button configured
  
  bool currentButtonState = digitalRead(navButtonPin);
  unsigned long currentTime = millis();
  
  // Check for button press (LOW state due to pull-up)
  if (currentButtonState == LOW && lastButtonState == HIGH && 
      currentTime - lastButtonPress > buttonDebounceTime) {
    nextPage();
    lastButtonPress = currentTime;
    Serial.printf("Navigation button pressed - Page %d/%d\n", currentPage + 1, totalPages);
  }
  
  lastButtonState = currentButtonState;
}

void OLEDDisplay::update(const AfterburnerSettings& settings, float throttle, bool bleConnected) {
  if (!displayEnabled) return;
  
  // Handle button input
  handleButton();
  
  unsigned long currentTime = millis();
  if (currentTime - lastUpdate < updateInterval) return;
  
  lastUpdate = currentTime;
  
  display->clearBuffer();
  
  switch (currentPage) {
    case 0:
      drawMainPage(settings, throttle, bleConnected);
      break;
    case 1:
      drawSettingsPage(settings);
      break;
    case 2:
      drawStatusPage(settings, throttle, bleConnected);
      break;
  }
  
  display->sendBuffer();
  
  // Auto-advance pages every 3 seconds (only if no button is configured)
  if (navButtonPin == 0) {
    static unsigned long lastPageChange = 0;
    if (currentTime - lastPageChange > 3000) {
      currentPage = (currentPage + 1) % totalPages;
      lastPageChange = currentTime;
    }
  }
}

void OLEDDisplay::drawMainPage(const AfterburnerSettings& settings, float throttle, bool bleConnected) {
  // Title - adjusted for 128x64 screen with offset
  display->setFont(u8g2_font_6x10_tr);
  display->drawStr(15, 24, "Afterburner");
  
  // Mode
  display->drawStr(15, 36, "Mode:");
  display->drawStr(50, 36, modeNames[settings.mode]);
  
  // Throttle
  display->drawStr(15, 48, "Throttle:");
  char throttleStr[10];
  formatThrottle(throttle, throttleStr);
  display->drawStr(70, 48, throttleStr);
  
  // Throttle bar
  drawThrottleBar(throttle);
  
  // Connection status
  drawConnectionStatus(bleConnected);
  
  // Page indicator
  display->drawStr(15, 62, "Page 1/3");
}

void OLEDDisplay::drawSettingsPage(const AfterburnerSettings& settings) {
  // Title - adjusted for 128x64 screen with offset
  display->setFont(u8g2_font_6x10_tr);
  display->drawStr(15, 24, "Settings");
  
  // Speed
  display->drawStr(15, 36, "Speed:");
  char speedStr[10];
  formatSpeed(settings.speedMs, speedStr);
  display->drawStr(60, 36, speedStr);
  
  // Brightness
  display->drawStr(15, 48, "Bright:");
  char brightStr[10];
  sprintf(brightStr, "%d", settings.brightness);
  display->drawStr(60, 48, brightStr);
  
  // LED Count
  display->drawStr(15, 60, "LEDs:");
  char ledStr[10];
  sprintf(ledStr, "%d", settings.numLeds);
  display->drawStr(50, 60, ledStr);
  
  // AB Threshold - moved to next line
  char abStr[20];
  sprintf(abStr, "AB:%d%%", settings.abThreshold);
  display->drawStr(80, 60, abStr);
  
  // Page indicator
  display->drawStr(15, 62, "Page 2/3");
}

void OLEDDisplay::drawStatusPage(const AfterburnerSettings& settings, float throttle, bool bleConnected) {
  // Title - adjusted for 128x64 screen with offset
  display->setFont(u8g2_font_6x10_tr);
  display->drawStr(15, 24, "Status");
  
  // Start Color
  display->drawStr(15, 36, "Start:");
  char startColorStr[20];
  sprintf(startColorStr, "R%d G%d B%d", 
          settings.startColor[0], 
          settings.startColor[1], 
          settings.startColor[2]);
  display->drawStr(50, 36, startColorStr);
  
  // End Color
  display->drawStr(15, 48, "End:");
  char endColorStr[20];
  sprintf(endColorStr, "R%d G%d B%d", 
          settings.endColor[0], 
          settings.endColor[1], 
          settings.endColor[2]);
  display->drawStr(45, 48, endColorStr);
  
  // Connection
  display->drawStr(15, 60, "BLE:");
  if (bleConnected) {
    display->drawStr(45, 60, "Connected");
  } else {
    display->drawStr(45, 60, "Disconnected");
  }
  
  // Page indicator
  display->drawStr(15, 62, "Page 3/3");
}

void OLEDDisplay::drawThrottleBar(float throttle) {
  // Draw throttle bar - adjusted for 128x64 screen with offset
  int barWidth = (int)(throttle * 80 / 100.0); // 80 pixels max width for 128x64
  display->drawFrame(15, 52, 90, 8); // Border
  if (barWidth > 0) {
    display->drawBox(16, 53, barWidth, 6); // Fill
  }
}

void OLEDDisplay::drawModeIndicator(uint8_t mode) {
  const char* indicators[3] = {"L", "E", "P"};
  display->drawStr(50, 22, indicators[mode]);
}

void OLEDDisplay::drawConnectionStatus(bool bleConnected) {
  if (bleConnected) {
    display->drawStr(15, 60, "BLE: ON");
  } else {
    display->drawStr(15, 60, "BLE: OFF");
  }
}

void OLEDDisplay::drawColorPreview(const uint8_t* color) {
  // Draw a small color preview box
  display->drawBox(50, 34, 8, 8);
}

void OLEDDisplay::formatThrottle(float throttle, char* buffer) {
  sprintf(buffer, "%d%%", (int)throttle);
}

void OLEDDisplay::formatSpeed(uint16_t speedMs, char* buffer) {
  if (speedMs >= 1000) {
    sprintf(buffer, "%.1fs", speedMs / 1000.0);
  } else {
    sprintf(buffer, "%dms", speedMs);
  }
}

void OLEDDisplay::setUpdateInterval(unsigned long interval) {
  updateInterval = interval;
}

void OLEDDisplay::enable(bool enabled) {
  displayEnabled = enabled;
  if (!enabled) {
    display->clearBuffer();
    display->sendBuffer();
  }
}

void OLEDDisplay::nextPage() {
  currentPage = (currentPage + 1) % totalPages;
}

void OLEDDisplay::setPage(uint8_t page) {
  if (page < totalPages) {
    currentPage = page;
  }
}

uint8_t OLEDDisplay::getCurrentPage() {
  return currentPage;
}
