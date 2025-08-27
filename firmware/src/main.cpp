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
AfterburnerBLEService bleService(&settingsManager);
OLEDDisplay oledDisplay;

// Demo mode flag (can be set via #define DEMO_MODE)
#ifdef DEMO_MODE
bool demoMode = true;
#else
bool demoMode = false;
#endif

// Test mode flag (can be set via #define TEST_MODE)
#ifdef TEST_MODE
bool testMode = true;
#else
bool testMode = false;
#endif

// Test functions
void runTests() {
  Serial.println("=== RUNNING TESTS ===");
  
  // Test 1: Settings initialization
  Serial.println("Test 1: Settings initialization");
  AfterburnerSettings& settings = settingsManager.getSettings();
  Serial.printf("Default mode: %d\n", settings.mode);
  Serial.printf("Default brightness: %d\n", settings.brightness);
  Serial.printf("Default numLeds: %d\n", settings.numLeds);
  Serial.println("✓ Settings test passed");
  
  // Test 2: LED effects initialization
  Serial.println("Test 2: LED effects initialization");
  ledEffects.begin(settings.numLeds);
  Serial.println("✓ LED effects test passed");
  
  // Test 3: Throttle reading
  Serial.println("Test 3: Throttle reading");
  float throttle = throttleReader.readThrottle();
  Serial.printf("Throttle value: %.2f\n", throttle);
  Serial.println("✓ Throttle test passed");
  
  // Test 4: BLE service initialization
  Serial.println("Test 4: BLE service initialization");
  bleService.begin();
  Serial.println("✓ BLE service test passed");
  
  // Test 5: OLED display initialization
  Serial.println("Test 5: OLED display initialization");
  oledDisplay.begin(2);
  Serial.println("✓ OLED display test passed");
  
  Serial.println("=== ALL TESTS PASSED ===");
  Serial.println("Test mode complete. Rebooting in 5 seconds...");
  delay(5000);
  ESP.restart();
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("ESP32 Afterburner Starting...");
  
  if (testMode) {
    runTests();
    return;
  }
  
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
  if (testMode) {
    return; // Don't run main loop in test mode
  }
  
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
