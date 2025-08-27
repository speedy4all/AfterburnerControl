#include <Arduino.h>
#include "settings.h"
#include "throttle.h"
#include "led_effects.h"
#include "ble_service.h"
#include "oled_display.h"

// Global objects
SettingsManager settingsManager;
ThrottleReader throttleReader;
LEDEffects ledEffects;
BLEService bleService(&settingsManager);
OLEDDisplay oledDisplay;

// Demo mode flag (can be set via #define DEMO_MODE)
#ifdef DEMO_MODE
bool demoMode = true;
#else
bool demoMode = false;
#endif

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("ESP32 Afterburner Starting...");
  
  // Initialize OLED display with dedicated navigation button on GPIO2
  // GPIO2 is a good choice as it's not used for I2C and is easily accessible
  oledDisplay.begin(2);
  Serial.println("OLED Display initialized with navigation button on GPIO2");
  
  // Initialize settings
  settingsManager.begin();
  AfterburnerSettings& settings = settingsManager.getSettings();
  
  // Initialize throttle reader
  throttleReader.begin();
  if (demoMode) {
    throttleReader.setDemoMode(true);
    Serial.println("Demo mode enabled");
  }
  
  // Initialize LED effects
  ledEffects.begin(settings.numLeds);
  
  // Initialize BLE service
  bleService.begin();
  
  Serial.println("ESP32 Afterburner Ready!");
}

void loop() {
  // Read throttle
  float throttle = throttleReader.readThrottle();
  
  // Get current settings
  AfterburnerSettings& settings = settingsManager.getSettings();
  
  // Update LED effects
  ledEffects.render(settings, throttle);
  
  // Update BLE status
  bleService.updateStatus(throttle, settings.mode);
  
  // Update OLED display (button handling is now done inside the display class)
  bool bleConnected = bleService.isConnected();
  oledDisplay.update(settings, throttle, bleConnected);
  
  // Handle LED count changes
  static uint16_t lastNumLeds = settings.numLeds;
  if (settings.numLeds != lastNumLeds) {
    ledEffects.update(settings.numLeds);
    lastNumLeds = settings.numLeds;
  }
  
  // Small delay to maintain frame rate
  delay(20); // ~50 FPS
}
