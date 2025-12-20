#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <Arduino.h>
#include "constants.h"

/**
 * LEDDriver - Controls 4 MOSFET channels via PWM
 * Each channel controls 9 LEDs in parallel (36 LEDs total)
 * Uses ESP32 LEDC (LED Control) hardware PWM for smooth control
 */
class LEDDriver {
private:
  uint8_t mosfetPins[4];           // GPIO pins for 4 MOSFETs
  uint8_t channelIntensities[4];  // 0-255 PWM values per channel
  uint8_t ledcChannels[4];        // LEDC channel assignments (0-3)
  bool initialized;

public:
  LEDDriver();
  ~LEDDriver();
  
  /**
   * Initialize PWM channels for all 4 MOSFETs
   * Must be called before using other methods
   */
  void begin();
  
  /**
   * Set intensity for a specific channel
   * @param channel Channel number (0-3)
   * @param intensity PWM value (0-255)
   */
  void setChannel(uint8_t channel, uint8_t intensity);
  
  /**
   * Set intensities for all channels at once
   * @param intensities Array of 4 intensity values (0-255)
   */
  void setAllChannels(uint8_t intensities[4]);
  
  /**
   * Get current intensity for a channel
   * @param channel Channel number (0-3)
   * @return Current intensity (0-255)
   */
  uint8_t getChannelIntensity(uint8_t channel);
  
  /**
   * Update PWM outputs (called automatically, but can be manual)
   * Note: ESP32 LEDC updates automatically, but this can be used for verification
   */
  void update();
  
  /**
   * Turn all LEDs off (set all channels to 0)
   */
  void clear();
  
  /**
   * Check if driver is initialized
   * @return true if initialized, false otherwise
   */
  bool isInitialized() const { return initialized; }

private:
  /**
   * Setup LEDC channel for a specific MOSFET pin
   * @param channel Channel index (0-3)
   * @param pin GPIO pin number
   */
  void setupLEDCChannel(uint8_t channel, uint8_t pin);
};

#endif // LED_DRIVER_H

