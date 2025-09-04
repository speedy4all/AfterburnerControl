#ifndef THROTTLE_H
#define THROTTLE_H

#include <Arduino.h>
#include "constants.h"

class ThrottleReader {
private:
  float smoothedThrottle;
  float alpha;  // Smoothing factor
  unsigned long lastPulseTime;
  bool demoMode;

public:
  ThrottleReader();
  void begin();
  float readThrottle();
  float getSmoothedThrottle();
  void setDemoMode(bool enabled);
  void updateDemoThrottle();
  
  // Throttle calibration methods
  void startCalibration();
  void stopCalibration();
  bool isCalibrating();
  void updateCalibration();
  uint16_t getCalibratedMin();
  uint16_t getCalibratedMax();
  bool isCalibrated();
  
  // Enhanced calibration progress methods
  uint8_t getMinVisits();
  uint8_t getMaxVisits();
  
  // Update calibration values from settings
  void updateCalibrationValues(uint16_t minPWM, uint16_t maxPWM);
  
    // Reset calibration to defaults
  void resetCalibrationToDefaults();
  
  // Debug: Get current calibration state
  void debugCalibrationState();
  
  private:
  float readPWM();
  float mapPWMToThrottle(unsigned long pulseWidth);
  
  // Calibration state
  bool calibrating;
  uint16_t calibrationMin;
  uint16_t calibrationMax;
  uint16_t calibrationSamples[CALIBRATION_SAMPLES];
  uint8_t sampleIndex;
  unsigned long calibrationStartTime;
  
  // Enhanced calibration tracking
  uint16_t minVisits;           // Number of times we've visited the min position
  uint16_t maxVisits;           // Number of times we've visited the max position
  uint16_t lastMinValue;        // Last min value detected
  uint16_t lastMaxValue;        // Last max value detected
  uint16_t minStabilityCount;   // Count of stable min readings
  uint16_t maxStabilityCount;   // Count of stable max readings
  unsigned long lastMinTime;    // Time of last min detection
  unsigned long lastMaxTime;    // Time of last max detection
};

#endif // THROTTLE_H
