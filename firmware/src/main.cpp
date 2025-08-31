#include <Arduino.h>
#include <ArduinoJson.h>
#include "settings.h"
#include "throttle.h"
#include "led_effects.h"
#include "wifi_service.h"

// Global objects
SettingsManager settingsManager;
ThrottleReader throttleReader;
LEDEffects ledEffects;
AfterburnerWiFiService wifiService(&settingsManager);

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
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("ESP8266 Afterburner Starting...");
  
  // Run hardware test only
  testHardware();
  
  if (testMode) {
    runTests();
    return;
  }
  
  // OLED display initialization removed
  Serial.println("OLED Display initialization skipped");
  
  // Initialize settings
  settingsManager.begin();
  AfterburnerSettings& settings = settingsManager.getSettings();
  
  // Force test colors for debugging
  settings.startColor[0] = 255; settings.startColor[1] = 0; settings.startColor[2] = 0;   // Red
  settings.endColor[0] = 0; settings.endColor[1] = 255; settings.endColor[2] = 0;         // Green
  settings.brightness = 255;
  Serial.println("Forced test colors: Red -> Green");
  
  // Initialize throttle reader
  throttleReader.begin();
  if (demoMode) {
    throttleReader.setDemoMode(true);
    Serial.println("Demo mode enabled");
  } else {
    // Disable demo mode to test real PWM
    throttleReader.setDemoMode(false);
    Serial.println("Demo mode disabled - testing real PWM signal");
    
    // Start automatic throttle calibration
    Serial.println("Starting automatic throttle calibration...");
    throttleReader.startCalibration();
  }
  
  // Initialize LED effects
  ledEffects.begin(settings.numLeds);
  
  // Initialize WiFi service
  wifiService.begin();
  
  Serial.println("ESP8266 Afterburner Ready!");
}

void loop() {
  if (testMode) {
    return; // Don't run main loop in test mode
  }
  
  // Read throttle
  float throttle = throttleReader.readThrottle();
  
  // Get current settings
  AfterburnerSettings& settings = settingsManager.getSettings();
  
  // Debug output every 2 seconds
  static unsigned long lastDebugTime = 0;
  if (millis() - lastDebugTime > 2000) {
    Serial.printf("Throttle: %.2f, Mode: %d, LEDs: %d", 
                 throttle, settings.mode, settings.numLeds);
    
    if (throttleReader.isCalibrating()) {
      Serial.printf(", Calibrating: %lu samples", throttleReader.getPulseCount());
    } else if (throttleReader.isCalibrationComplete()) {
      Serial.printf(", Calibrated: %lu-%lu us", 
                   throttleReader.getMinPulse(), throttleReader.getMaxPulse());
    }
    
    Serial.println();
    lastDebugTime = millis();
  }
  
  // Update LED effects
  ledEffects.render(settings, throttle);
  
  // Update WiFi status
  wifiService.updateStatus(throttle, settings.mode);
  
  // Handle WiFi service loop
  wifiService.loop();
  
  // Handle LED count changes
  static uint16_t lastNumLeds = settings.numLeds;
  if (settings.numLeds != lastNumLeds) {
    ledEffects.update(settings.numLeds);
    lastNumLeds = settings.numLeds;
  }
  
  // Small delay to maintain frame rate
  delay(20); // ~50 FPS
}
