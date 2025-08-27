#ifndef THROTTLE_H
#define THROTTLE_H

#include <Arduino.h>

// Pin definitions
#define THR_PIN 34
#define PWM_TIMEOUT 25000  // 25ms timeout for PWM read

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
  
private:
  float readPWM();
  float mapPWMToThrottle(unsigned long pulseWidth);
};

#endif // THROTTLE_H
