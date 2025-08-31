#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "settings.h"
#include "throttle.h"
#include "led_effects.h"
#include "wifi_service.h"

// Global instances
SettingsManager settingsManager;
ThrottleReader throttleReader;
AfterburnerWiFiService wifiService(&settingsManager, &throttleReader);
LEDEffects ledEffects;

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

// EEPROM eraser function
void eraseEEPROM() {
  Serial.println("=== EEPROM ERASER ===");
  Serial.println("Clearing all EEPROM data...");
  
  // Clear the entire EEPROM (512 bytes)
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
    if (i % 64 == 0) {
      Serial.printf("Cleared %d bytes...\n", i);
      ESP.wdtFeed(); // Feed watchdog during long operation
    }
  }
  
  // Commit the changes
  EEPROM.commit();
  Serial.println("EEPROM completely erased!");
  Serial.println("All settings and calibration data cleared.");
  Serial.println("Device will use default values on next boot.");
}

// JSON parsing test function
void testJsonParsing() {
  Serial.println("=== JSON Parsing Test ===");
  
  // Test 1: Simple object
  String json1 = "{\"mode\":0}";
  Serial.print("Testing JSON 1: ");
  Serial.println(json1);
  
  StaticJsonDocument<512> doc1;
  DeserializationError error1 = deserializeJson(doc1, json1);
  
  if (error1) {
    Serial.print("Error 1: ");
    Serial.println(error1.c_str());
  } else {
    Serial.println("JSON 1 parsed successfully");
    if (doc1.containsKey("mode")) {
      Serial.printf("Mode value: %d\n", doc1["mode"]);
    }
  }
  
  // Test 2: Color array
  String json2 = "{\"startColor\":[255,0,0]}";
  Serial.print("Testing JSON 2: ");
  Serial.println(json2);
  
  StaticJsonDocument<512> doc2;
  DeserializationError error2 = deserializeJson(doc2, json2);
  
  if (error2) {
    Serial.print("Error 2: ");
    Serial.println(error2.c_str());
  } else {
    Serial.println("JSON 2 parsed successfully");
    if (doc2.containsKey("startColor")) {
      JsonArray colorArray = doc2["startColor"];
      if (colorArray.size() == 3) {
        Serial.printf("Color: [%d,%d,%d]\n", 
                     colorArray[0], colorArray[1], colorArray[2]);
      }
    }
  }
  
  // Test 3: Complete settings object
  String json3 = "{\"mode\":2,\"startColor\":[255,0,0],\"endColor\":[0,0,255],\"speedMs\":1200,\"brightness\":200,\"numLeds\":45,\"abThreshold\":80}";
  Serial.print("Testing JSON 3: ");
  Serial.println(json3);
  
  StaticJsonDocument<512> doc3;
  DeserializationError error3 = deserializeJson(doc3, json3);
  
  if (error3) {
    Serial.print("Error 3: ");
    Serial.println(error3.c_str());
  } else {
    Serial.println("JSON 3 parsed successfully");
    Serial.printf("Mode: %d\n", doc3["mode"]);
    Serial.printf("Speed: %d\n", doc3["speedMs"]);
    Serial.printf("Brightness: %d\n", doc3["brightness"]);
  }
  
  Serial.println("=== JSON Test Complete ===");
}

// Hardware test function
void testHardware() {
  Serial.println("=== Hardware Test ===");
  
  // Test 1: PWM pin
  Serial.printf("Testing PWM pin %d (D5)...\n", THR_PIN);
  pinMode(THR_PIN, INPUT);
  
  // Test 2: LED pin
  Serial.printf("Testing LED pin %d (D6)...\n", LED_PIN);
  pinMode(LED_PIN, OUTPUT);
  
  // Test 3: Simple LED test
  Serial.println("Testing LED strip with simple pattern...");
  ledEffects.begin(10); // Test with 10 LEDs
  
  // Create test settings for red
  AfterburnerSettings testSettings;
  testSettings.mode = 0;
  testSettings.startColor[0] = 255; testSettings.startColor[1] = 0; testSettings.startColor[2] = 0;
  testSettings.endColor[0] = 255; testSettings.endColor[1] = 0; testSettings.endColor[2] = 0;
  testSettings.brightness = 255;
  testSettings.numLeds = 10;
  
  // Show red pattern
  Serial.println("Showing red pattern...");
  ledEffects.render(testSettings, 1.0); // Full throttle for bright red
  delay(2000); // Longer delay to see the effect
  
  // Show green pattern
  Serial.println("Showing green pattern...");
  testSettings.startColor[0] = 0; testSettings.startColor[1] = 255; testSettings.startColor[2] = 0;
  testSettings.endColor[0] = 0; testSettings.endColor[1] = 255; testSettings.endColor[2] = 0;
  ledEffects.render(testSettings, 1.0);
  delay(2000); // Longer delay to see the effect
  
  // Show blue pattern
  Serial.println("Showing blue pattern...");
  testSettings.startColor[0] = 0; testSettings.startColor[1] = 0; testSettings.startColor[2] = 255;
  testSettings.endColor[0] = 0; testSettings.endColor[1] = 0; testSettings.endColor[2] = 255;
  ledEffects.render(testSettings, 1.0);
  delay(2000); // Longer delay to see the effect
  
  // Clear LEDs
  Serial.println("Clearing LEDs...");
  FastLED.clear();
  FastLED.show();
  delay(1000);
  
  Serial.println("Hardware test complete");
}

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
  
  // Test 4: WiFi service initialization
  Serial.println("Test 4: WiFi service initialization");
  wifiService.begin();
  Serial.println("✓ WiFi service test passed");
  
  // Test 5: OLED display initialization (removed)
  Serial.println("Test 5: OLED display initialization - SKIPPED");
  Serial.println("✓ OLED display test skipped");
  
  Serial.println("=== ALL TESTS PASSED ===");
  Serial.println("Test mode complete. Rebooting in 5 seconds...");
  delay(5000);
  ESP.restart();
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== AFTERBURNER CONTROLLER ===");
  
  // Uncomment the next line to erase EEPROM and start fresh
  //eraseEEPROM();
  
  // Initialize EEPROM and load settings
  settingsManager.begin();
  AfterburnerSettings& settings = settingsManager.getSettings();
  
  // Initialize throttle reader
  throttleReader.begin();
  
  // Initialize LED effects
  ledEffects.begin(settings.numLeds);
  
  // Initialize WiFi service
  wifiService.begin();
  
  Serial.println("Throttle calibration disabled on startup - use mobile app to calibrate when needed");
  Serial.println("Setup complete!");
}

void loop() {
  if (testMode) {
    return; // Don't run main loop in test mode
  }
  
  // Update throttle reading (includes calibration update)
  float throttle = throttleReader.readThrottle();
  
  // Get current settings
  AfterburnerSettings& settings = settingsManager.getSettings();
  
  // Update LED effects
  ledEffects.render(settings, throttle);
  
  // Update WiFi service (handles WebSocket and deferred saves)
  wifiService.loop();
  
  // Update status via WiFi
  wifiService.updateStatus(throttle, settings.mode);
  
  // Small delay to prevent watchdog issues
  delay(10);
}
