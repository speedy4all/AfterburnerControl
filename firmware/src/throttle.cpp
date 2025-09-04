#include "throttle.h"

ThrottleReader::ThrottleReader() {
  smoothedThrottle = 0.0f;
  alpha = 0.10f;  // Smoothing factor
  lastPulseTime = 0;
  demoMode = false;
  
  // Initialize calibration state
  calibrating = false;
  calibrationMin = DEFAULT_THROTTLE_MIN;
  calibrationMax = DEFAULT_THROTTLE_MAX;
  sampleIndex = 0;
  calibrationStartTime = 0;
  
  // Initialize calibration samples array
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    calibrationSamples[i] = 0;
  }
  
  // Initialize enhanced calibration tracking
  minVisits = 0;
  maxVisits = 0;
  lastMinValue = DEFAULT_THROTTLE_MIN;
  lastMaxValue = DEFAULT_THROTTLE_MAX;
  minStabilityCount = 0;
  maxStabilityCount = 0;
  lastMinTime = 0;
  lastMaxTime = 0;
}

void ThrottleReader::begin() {
  pinMode(THROTTLE_PIN, INPUT);
  
  // Note: Calibration values will be loaded from settings manager
  // after the settings manager is initialized
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
  unsigned long pulseWidth = pulseIn(THROTTLE_PIN, HIGH, PWM_TIMEOUT);
  
  if (pulseWidth == 0) {
    // No pulse detected, keep last value (failsafe)
    return smoothedThrottle;
  }
  
  return mapPWMToThrottle(pulseWidth);
}

float ThrottleReader::mapPWMToThrottle(unsigned long pulseWidth) {
  // Check if we have valid calibration (min < max and reasonable range)
  bool hasValidCalibration = (calibrationMin < calibrationMax) && 
                            (calibrationMax - calibrationMin) > 500 &&
                            (calibrationMin >= MIN_PWM_VALUE) && 
                            (calibrationMax <= MAX_PWM_VALUE);
  
  // Use calibrated values if available, otherwise use defaults
  uint16_t minPWM = hasValidCalibration ? calibrationMin : DEFAULT_THROTTLE_MIN;
  uint16_t maxPWM = hasValidCalibration ? calibrationMax : DEFAULT_THROTTLE_MAX;
  

  
  // Clamp to calibrated range
  if (pulseWidth < minPWM) pulseWidth = minPWM;
  if (pulseWidth > maxPWM) pulseWidth = maxPWM;
  
  // Map to 0.0-1.0 range
  float result = (float)(pulseWidth - minPWM) / (float)(maxPWM - minPWM);
  
  // Ensure result is in valid range
  if (result < 0.0f) result = 0.0f;
  if (result > 1.0f) result = 1.0f;
  
  return result;
}

// Throttle calibration methods
void ThrottleReader::startCalibration() {
  Serial.println("🎯 Starting throttle calibration...");
  calibrating = true;
  sampleIndex = 0;
  calibrationStartTime = millis();
  
  // Reset calibration values
  calibrationMin = MAX_PWM_VALUE;
  calibrationMax = MIN_PWM_VALUE;
  
  // Clear samples array
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    calibrationSamples[i] = 0;
  }
  
  // Reset enhanced calibration tracking
  minVisits = 0;
  maxVisits = 0;
  lastMinValue = MAX_PWM_VALUE;
  lastMaxValue = MIN_PWM_VALUE;
  minStabilityCount = 0;
  maxStabilityCount = 0;
  lastMinTime = 0;
  lastMaxTime = 0;
  
  Serial.println("Calibration started - move throttle to min and max positions multiple times");
}

void ThrottleReader::stopCalibration() {
  if (calibrating) {
    calibrating = false;
  }
}

bool ThrottleReader::isCalibrating() {
  return calibrating;
}

void ThrottleReader::updateCalibration() {
  if (!calibrating) return;
  
  // Check for timeout
  if (millis() - calibrationStartTime > CALIBRATION_TIMEOUT) {
    stopCalibration();
    return;
  }
  
  // Read current PWM value
  unsigned long pulseWidth = pulseIn(THROTTLE_PIN, HIGH, PWM_TIMEOUT);
  
  if (pulseWidth > 0 && pulseWidth >= MIN_PWM_VALUE && pulseWidth <= MAX_PWM_VALUE) {
    // Add sample to array
    calibrationSamples[sampleIndex] = pulseWidth;
    sampleIndex = (sampleIndex + 1) % CALIBRATION_SAMPLES;
    
    // Update min/max values
    if (pulseWidth < calibrationMin) {
      calibrationMin = pulseWidth;
    }
    if (pulseWidth > calibrationMax) {
      calibrationMax = pulseWidth;
    }
    
    // Enhanced calibration logic - track visits to min and max positions
    unsigned long currentTime = millis();
    
    // Check for min position visit
    if (pulseWidth <= calibrationMin + MIN_STABILITY_THRESHOLD) {
      // Check if enough time has passed since last min visit
      if (currentTime - lastMinTime > TIME_BETWEEN_VISITS) {
        // Check if this is a new min position (within threshold)
        if (abs((int)pulseWidth - (int)lastMinValue) > MIN_STABILITY_THRESHOLD) {
          minVisits++;
          lastMinValue = pulseWidth;
          lastMinTime = currentTime;
        }
      }
    }
    
    // Check for max position visit
    if (pulseWidth >= calibrationMax - MIN_STABILITY_THRESHOLD) {
      // Check if enough time has passed since last max visit
      if (currentTime - lastMaxTime > TIME_BETWEEN_VISITS) {
        // Check if this is a new max position (within threshold)
        if (abs((int)pulseWidth - (int)lastMaxValue) > MIN_STABILITY_THRESHOLD) {
          maxVisits++;
          lastMaxValue = pulseWidth;
          lastMaxTime = currentTime;
        }
      }
    }
    
    // Check if calibration is complete
    if (minVisits >= MIN_VISITS_REQUIRED && maxVisits >= MAX_VISITS_REQUIRED && 
        (calibrationMax - calibrationMin) > 500) {
      Serial.printf("Calibration complete! Min: %u μs, max: %u μs\n", calibrationMin, calibrationMax);
      stopCalibration();
    }
    
    // Progress update every 5 seconds
    static unsigned long lastProgressUpdate = 0;
    if (currentTime - lastProgressUpdate > 5000) {
      Serial.printf("🎯 Calibration progress - Min visits: %d/%d, Max visits: %d/%d, Range: %u μs\n",
                    minVisits, MIN_VISITS_REQUIRED, maxVisits, MAX_VISITS_REQUIRED,
                    calibrationMax - calibrationMin);
      lastProgressUpdate = currentTime;
    }
  }
}

uint16_t ThrottleReader::getCalibratedMin() {
  return calibrationMin;
}

uint16_t ThrottleReader::getCalibratedMax() {
  return calibrationMax;
}

bool ThrottleReader::isCalibrated() {
  return !calibrating && (calibrationMax - calibrationMin) > 500;
}

uint8_t ThrottleReader::getMinVisits() {
  return minVisits;
}

uint8_t ThrottleReader::getMaxVisits() {
  return maxVisits;
}

void ThrottleReader::updateCalibrationValues(uint16_t minPWM, uint16_t maxPWM) {
  Serial.printf("Throttle: Updating calibration values - Min: %u, Max: %u\n", minPWM, maxPWM);
  
  // Validate the new values
  if (minPWM >= maxPWM || minPWM < MIN_PWM_VALUE || maxPWM > MAX_PWM_VALUE) {
    Serial.printf("Throttle: ❌ Invalid calibration values - Min: %u, Max: %u\n", minPWM, maxPWM);
    return;
  }
  
  // Update the calibration values
  calibrationMin = minPWM;
  calibrationMax = maxPWM;
  
  Serial.printf("Throttle: ✅ Calibration values updated successfully - Min: %u, Max: %u\n", 
                calibrationMin, calibrationMax);
  
  // Debug: Verify the values were actually set
  Serial.printf("Throttle: 🔍 Verification - Internal calibrationMin: %u, calibrationMax: %u\n", 
                this->calibrationMin, this->calibrationMax);
}

void ThrottleReader::resetCalibrationToDefaults() {
  Serial.println("Throttle: 🔄 Resetting calibration to defaults...");
  
  // Reset to default values
  calibrationMin = DEFAULT_THROTTLE_MIN;
  calibrationMax = DEFAULT_THROTTLE_MAX;
  
  Serial.printf("Throttle: ✅ Calibration reset to defaults - Min: %u, Max: %u\n", 
                calibrationMin, calibrationMax);
}

void ThrottleReader::debugCalibrationState() {
  Serial.printf("Throttle: 🔍 Debug - Calibrating: %s, Min: %u, Max: %u, Range: %u\n",
                calibrating ? "true" : "false", calibrationMin, calibrationMax, 
                (calibrationMax > calibrationMin) ? (calibrationMax - calibrationMin) : 0);
  
  if (calibrating) {
    Serial.printf("Throttle: 🔍 Calibration progress - Min visits: %d/%d, Max visits: %d/%d\n",
                  minVisits, MIN_VISITS_REQUIRED, maxVisits, MAX_VISITS_REQUIRED);
    Serial.printf("Throttle: 🔍 Last min: %u, Last max: %u\n", lastMinValue, lastMaxValue);
  }
  
  bool hasValidCalibration = (calibrationMin < calibrationMax) && 
                            (calibrationMax - calibrationMin) > 500 &&
                            (calibrationMin >= MIN_PWM_VALUE) && 
                            (calibrationMax <= MAX_PWM_VALUE);
  
  Serial.printf("Throttle: 🔍 Valid calibration: %s\n", hasValidCalibration ? "true" : "false");
}
