#ifndef THROTTLE_H
#define THROTTLE_H

#include <Arduino.h>

// Pin definitions for ESP8266 NodeMCU V3
#define THR_PIN 14  // D5 - GPIO14
#define PWM_TIMEOUT 50000  // 50ms timeout for PWM read (increased for reliability)
#define PWM_MIN_PULSE 900   // Minimum valid pulse width (microseconds) - fallback value
#define PWM_MAX_PULSE 2100  // Maximum valid pulse width (microseconds) - fallback value
#define PWM_SIGNAL_TIMEOUT 1000  // 1 second timeout for signal loss detection
#define CALIBRATION_SAMPLES 100  // Number of samples to collect for calibration

class ThrottleReader {
private:
  float smoothedThrottle;
  float alpha;  // Smoothing factor
  unsigned long lastPulseTime;
  unsigned long lastValidPulseTime;
  bool demoMode;
  bool signalValid;
  unsigned long pulseCount;
  unsigned long invalidPulseCount;
  
  // Calibration variables
  bool calibrationMode;
  bool calibrationComplete;
  unsigned long calibrationSamples;
  unsigned long minPulse;
  unsigned long maxPulse;
  unsigned long calibrationStartTime;

public:
  ThrottleReader();
  void begin();
  float readThrottle();
  float getSmoothedThrottle();
  void setDemoMode(bool enabled);
  void updateDemoThrottle();
  bool isSignalValid() { return signalValid; }
  unsigned long getPulseCount() { return pulseCount; }
  unsigned long getInvalidPulseCount() { return invalidPulseCount; }
  
  // Calibration functions
  void startCalibration();
  void resetCalibration();
  bool isCalibrating() { return calibrationMode; }
  bool isCalibrationComplete() { return calibrationComplete; }
  unsigned long getMinPulse() { return minPulse; }
  unsigned long getMaxPulse() { return maxPulse; }
  
  // Calibration persistence
  void saveCalibration();
  void loadCalibration();
  
private:
  float readPWM();
  float mapPWMToThrottle(unsigned long pulseWidth);
  bool validatePulse(unsigned long pulseWidth);
  void updateCalibration(unsigned long pulseWidth);
};

#endif // THROTTLE_H
