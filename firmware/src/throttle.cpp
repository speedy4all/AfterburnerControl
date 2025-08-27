#include "throttle.h"

ThrottleReader::ThrottleReader() {
  smoothedThrottle = 0.0f;
  alpha = 0.10f;  // Smoothing factor
  lastPulseTime = 0;
  demoMode = false;
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
  
  if (pulseWidth == 0) {
    // No pulse detected, keep last value (failsafe)
    return smoothedThrottle;
  }
  
  return mapPWMToThrottle(pulseWidth);
}

float ThrottleReader::mapPWMToThrottle(unsigned long pulseWidth) {
  // Map 1000-2000 microseconds to 0.0-1.0
  if (pulseWidth < 1000) pulseWidth = 1000;
  if (pulseWidth > 2000) pulseWidth = 2000;
  
  return (float)(pulseWidth - 1000) / 1000.0f;
}
