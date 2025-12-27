#include <Arduino.h>
#include "constants.h"
#include "settings.h"
#include "throttle.h"
#include "led_effects.h"
#include "ble_service.h"

// Global objects
SettingsManager settingsManager;
ThrottleReader throttleReader;
LEDEffects ledEffects;
AfterburnerBLEService bleService(&settingsManager, &throttleReader);

// Global calibration flag
volatile bool startCalibrationFlag = false;

// Function to start throttle calibration (called from BLE service)
void startThrottleCalibration() {
  startCalibrationFlag = true;
}

// Debug: Check if BLE service object was created
void checkBLEServiceObject() {
  // Removed excessive debug prints
}

// Demo mode flag (can be set via #define DEMO_MODE)
#ifdef DEMO_MODE
bool demoMode = true;
#else
bool demoMode = false;
#endif

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(INITIAL_DELAY_MS);
  
  Serial.println("ESP32-C3 SuperMini Afterburner Starting...");
  
  // Initialize GPIO pins
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  pinMode(THROTTLE_PIN, INPUT);
  pinMode(LED_STRIP_PIN, OUTPUT);
  
  Serial.println("GPIO pins initialized");
  
  // Initialize components
  Serial.println("Initializing components...");
  
  settingsManager.begin();
  
  // Debug: Check BLE service object
  checkBLEServiceObject();
  
  // Verify settings manager initialization
  if (settingsManager.isInitialized()) {
    Serial.println("Settings manager initialized successfully");
    
    if (settingsManager.hasSavedSettings()) {
      Serial.println("Found saved settings in flash memory");
      // Check flash memory status
      settingsManager.checkFlashStatus();
    } else {
      Serial.println("No saved settings found - will use defaults (normal on first boot)");
    }
  } else {
    Serial.println("Settings manager failed to initialize properly!");
  }
  
  throttleReader.begin();
  
  // Load saved calibration values from settings manager
  if (settingsManager.isInitialized() && settingsManager.isThrottleCalibrated()) {
    uint16_t savedMin = settingsManager.getThrottleMin();
    uint16_t savedMax = settingsManager.getThrottleMax();
    Serial.printf("Loading saved throttle calibration - Min: %u, Max: %u\n", savedMin, savedMax);
    throttleReader.updateCalibrationValues(savedMin, savedMax);
    
    // Update BLE calibration status with saved values
    bleService.updateThrottleCalibrationStatus(true, savedMin, savedMax);
  } else {
    Serial.println("No saved throttle calibration found, using defaults");
    
    // Update BLE calibration status with default values
    bleService.updateThrottleCalibrationStatus(false, DEFAULT_THROTTLE_MIN, DEFAULT_THROTTLE_MAX);
  }
  
  // Set demo mode if enabled
  throttleReader.setDemoMode(demoMode);
  
  // Get LED count from settings (per ring) and initialize for dual turbines
  // Total LEDs = numLeds * 2 (one ring continues the first)
  uint16_t numLeds = settingsManager.getSettings().numLeds;
  ledEffects.begin(numLeds * 2);  // Dual turbine support: 2 rings
  
  try {
    bleService.begin();
    Serial.println("BLE service initialized successfully");
  } catch (...) {
    Serial.println("BLE service initialization failed!");
  }
  
  Serial.printf("LED count: %d, Demo mode: %s\n", numLeds, demoMode ? "enabled" : "disabled");
  Serial.println("ESP32-C3 SuperMini Afterburner Ready!");
  
  // Initial LED test
  digitalWrite(ONBOARD_LED_PIN, HIGH);
  delay(LED_TEST_DELAY_MS);
  digitalWrite(ONBOARD_LED_PIN, LOW);
}

void loop() {
  // Read throttle
  float throttle = throttleReader.readThrottle();
  
  // Debug: Log throttle value every 2 seconds (only if NaN)
  static unsigned long lastThrottleLog = 0;
  if (millis() - lastThrottleLog > 2000) {
    if (isnan(throttle)) {
      Serial.println("Throttle reading: NaN (calibration may be needed)");
      // Also debug the calibration state when we get NaN
      throttleReader.debugCalibrationState();
    }
    lastThrottleLog = millis();
  }
  
  // Check if calibration should start
  if (startCalibrationFlag) {
    Serial.println("Starting throttle calibration from BLE command...");
    throttleReader.startCalibration();
    startCalibrationFlag = false;
  }
  
  // Update throttle calibration if active
  if (throttleReader.isCalibrating()) {
    throttleReader.updateCalibration();
    
    // Send periodic calibration status updates during calibration
    static unsigned long lastCalibrationStatusUpdate = 0;
    if (millis() - lastCalibrationStatusUpdate > 1000) { // Update every second during calibration
      uint16_t currentMin = throttleReader.getCalibratedMin();
      uint16_t currentMax = throttleReader.getCalibratedMax();
      uint8_t minVisits = throttleReader.getMinVisits();
      uint8_t maxVisits = throttleReader.getMaxVisits();
      bleService.updateThrottleCalibrationProgress(currentMin, currentMax, minVisits, maxVisits);
      lastCalibrationStatusUpdate = millis();
    }
    
    // Check if calibration is complete
    if (throttleReader.isCalibrated()) {
      uint16_t minPWM = throttleReader.getCalibratedMin();
      uint16_t maxPWM = throttleReader.getCalibratedMax();
      
      Serial.printf("Calibration complete! Min: %u, Max: %u\n", minPWM, maxPWM);
      
      // Save calibration to settings
      settingsManager.updateThrottleCalibration(minPWM, maxPWM);
      
      // Update throttle reader with the new calibration values
      throttleReader.updateCalibrationValues(minPWM, maxPWM);
      
      // Update BLE calibration status characteristic with final values
      bleService.updateThrottleCalibrationStatus(true, minPWM, maxPWM);
      
      // Send an additional notification to ensure the app gets the update
      delay(100); // Small delay to ensure the first update is processed
      bleService.notifyCalibrationStatus();
      
      // Stop calibration
      throttleReader.stopCalibration();
    }
  }
  
  // Update LED effects using render method
  // The speedMs setting from settings controls animation timing for:
  // - Pulse mode afterburner effects
  // - Breathing effects in Ease and Pulse modes  
  // - Flicker animation speed
  // - Sparkle frequency during afterburner
  ledEffects.render(settingsManager.getSettings(), throttle);
  
  // Update BLE service
  uint8_t currentMode = settingsManager.getSettings().mode;
  bleService.updateStatus(throttle, currentMode);
  
  // Log mode changes only when they occur
  static unsigned long lastModeLog = 0;
  static uint8_t lastLoggedMode = 255; // Track if mode changed
  if (millis() - lastModeLog > 5000) {
    if (currentMode != lastLoggedMode) {
      Serial.printf("Mode changed: %d -> %d\n", lastLoggedMode, currentMode);
      lastLoggedMode = currentMode;
    }
    lastModeLog = millis();
  }
  
  // Check flash memory status every 30 seconds
  static unsigned long lastFlashCheck = 0;
  if (millis() - lastFlashCheck > 30000) {
    // Only check if settings manager is properly initialized
    if (settingsManager.isInitialized()) {
      if (settingsManager.hasSavedSettings()) {
        settingsManager.checkFlashStatus();
      }
    }
    lastFlashCheck = millis();
  }
  
  // Blink onboard LED every 2 seconds to show activity
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  unsigned long currentTime = millis();
  
  if (currentTime - lastBlink > STATUS_UPDATE_INTERVAL_MS) {
    ledState = !ledState;
    digitalWrite(ONBOARD_LED_PIN, ledState);
    lastBlink = currentTime;
    
    // // Print status every 2 seconds
    // Serial.printf("Status - Throttle: %.1f%%, Mode: %d, Free Heap: %lu bytes\n", 
    //               throttle * 100, settingsManager.getSettings().mode, ESP.getFreeHeap());
    
    // Show BLE connection status
    if (bleService.isConnected()) {
      Serial.println("BLE: Client connected");
    } else {
      bleService.ensureAdvertising();
    }
   
  }
  
  delay(LOOP_DELAY_MS); // Small delay for stability
}
