#include "throttle.h"

ThrottleReader::ThrottleReader() {
  smoothedThrottle = 0.0f;
  alpha = 0.50f;  // Increased smoothing factor for faster response
  lastPulseTime = 0;
  lastValidPulseTime = 0;
  demoMode = false;
  signalValid = false;
  pulseCount = 0;
  invalidPulseCount = 0;
  
  // Initialize calibration variables
  calibrationMode = false;
  calibrationComplete = false;
  calibrationSamples = 0;
  minPulse = 0;
  maxPulse = 0;
  calibrationStartTime = 0;
}

void ThrottleReader::begin() {
  pinMode(THR_PIN, INPUT);
}

float ThrottleReader::readThrottle() {
  if (demoMode) {
    updateDemoThrottle();
    return smoothedThrottle;
  }
  
  float currentThrottle = readPWM();
  
  // Apply low-pass smoothing
  smoothedThrottle = smoothedThrottle + alpha * (currentThrottle - smoothedThrottle);
  
  return smoothedThrottle;
}

float ThrottleReader::getSmoothedThrottle() {
  return smoothedThrottle;
}

void ThrottleReader::setDemoMode(bool enabled) {
  demoMode = enabled;
  if (enabled) {
    smoothedThrottle = 0.0f;
  }
}

void ThrottleReader::updateDemoThrottle() {
  // Demo mode: sweep throttle from 0 to 1 and back
  static float demoDirection = 1.0f;
  static unsigned long lastDemoUpdate = 0;
  
  if (millis() - lastDemoUpdate > 50) {  // Update every 50ms
    smoothedThrottle += demoDirection * 0.02f;
    
    if (smoothedThrottle >= 1.0f) {
      smoothedThrottle = 1.0f;
      demoDirection = -1.0f;
    } else if (smoothedThrottle <= 0.0f) {
      smoothedThrottle = 0.0f;
      demoDirection = 1.0f;
    }
    
    lastDemoUpdate = millis();
  }
}

float ThrottleReader::readPWM() {
  unsigned long pulseWidth = pulseIn(THR_PIN, HIGH, PWM_TIMEOUT);
  pulseCount++;
  
  // Update calibration if in calibration mode
  if (calibrationMode) {
    updateCalibration(pulseWidth);
  }
  
  // Debug output every 3 seconds
  static unsigned long lastDebugTime = 0;
  if (millis() - lastDebugTime > 3000) {
    float mappedValue = mapPWMToThrottle(pulseWidth);
    Serial.printf("PWM: %lu us -> %.2f throttle\n", pulseWidth, mappedValue);
    lastDebugTime = millis();
  }
  
  if (pulseWidth == 0) {
    // No pulse detected
    invalidPulseCount++;
    signalValid = false;
    return smoothedThrottle; // Keep last value (failsafe)
  }
  
  // Validate the pulse
  if (validatePulse(pulseWidth)) {
    lastValidPulseTime = millis();
    bool wasValid = signalValid;
    signalValid = true;
    float newThrottle = mapPWMToThrottle(pulseWidth);
    
    // If we just got a valid signal after having no signal, reset smoothing
    if (!wasValid) {
      smoothedThrottle = newThrottle;
    }
    
    return newThrottle;
  } else {
    invalidPulseCount++;
    signalValid = false;
    return smoothedThrottle; // Keep last value
  }
}

float ThrottleReader::mapPWMToThrottle(unsigned long pulseWidth) {
  // Use calibrated values if available, otherwise use defaults
  unsigned long minPulse = calibrationComplete ? this->minPulse : 900;
  unsigned long maxPulse = calibrationComplete ? this->maxPulse : 2100;
  
  // Constrain pulse width to valid range
  if (pulseWidth < minPulse) pulseWidth = minPulse;
  if (pulseWidth > maxPulse) pulseWidth = maxPulse;
  
  return (float)(pulseWidth - minPulse) / (maxPulse - minPulse);
}

bool ThrottleReader::validatePulse(unsigned long pulseWidth) {
  // During calibration, accept a wider range
  if (calibrationMode) {
    return (pulseWidth >= 500 && pulseWidth <= 2500);
  }
  
  // Use calibrated values if available, otherwise use defaults
  unsigned long minPulse = calibrationComplete ? this->minPulse : PWM_MIN_PULSE;
  unsigned long maxPulse = calibrationComplete ? this->maxPulse : PWM_MAX_PULSE;
  
  // Check if pulse is within valid range
  if (pulseWidth < minPulse || pulseWidth > maxPulse) {
    return false;
  }
  
  return true;
}

void ThrottleReader::startCalibration() {
  calibrationMode = true;
  calibrationComplete = false;
  calibrationSamples = 0;
  minPulse = 0xFFFFFFFF;  // Start with maximum value
  maxPulse = 0;           // Start with minimum value
  calibrationStartTime = millis();
  
  Serial.println("=== THROTTLE CALIBRATION STARTED ===");
  Serial.println("Move your throttle stick from MIN to MAX several times");
  Serial.println("Calibration will complete automatically after 100 samples");
}

void ThrottleReader::updateCalibration(unsigned long pulseWidth) {
  if (pulseWidth == 0) return;  // Skip invalid pulses
  
  // Update min/max values
  if (pulseWidth < minPulse) {
    minPulse = pulseWidth;
  }
  if (pulseWidth > maxPulse) {
    maxPulse = pulseWidth;
  }
  
  calibrationSamples++;
  
  // Debug output every 10 samples
  if (calibrationSamples % 10 == 0) {
    Serial.printf("Calibration: %lu samples, Min: %lu us, Max: %lu us\n", 
                 calibrationSamples, minPulse, maxPulse);
  }
  
  // Check if calibration is complete
  if (calibrationSamples >= CALIBRATION_SAMPLES) {
    calibrationMode = false;
    calibrationComplete = true;
    
    Serial.println("=== THROTTLE CALIBRATION COMPLETE ===");
    Serial.printf("Min Pulse: %lu us\n", minPulse);
    Serial.printf("Max Pulse: %lu us\n", maxPulse);
    Serial.printf("Range: %lu us\n", maxPulse - minPulse);
    Serial.println("Calibration saved!");
  }
}
