#include "led_driver.h"
#include <driver/ledc.h>

LEDDriver::LEDDriver() {
  // Initialize pin assignments
  mosfetPins[0] = MOSFET_1_PIN;
  mosfetPins[1] = MOSFET_2_PIN;
  mosfetPins[2] = MOSFET_3_PIN;
  mosfetPins[3] = MOSFET_4_PIN;
  
  // Initialize LEDC channel assignments (use channels 0-3)
  ledcChannels[0] = LEDC_CHANNEL_0;
  ledcChannels[1] = LEDC_CHANNEL_1;
  ledcChannels[2] = LEDC_CHANNEL_2;
  ledcChannels[3] = LEDC_CHANNEL_3;
  
  // Initialize intensities to 0
  for (int i = 0; i < 4; i++) {
    channelIntensities[i] = 0;
  }
  
  initialized = false;
}

LEDDriver::~LEDDriver() {
  // Cleanup if needed
  if (initialized) {
    clear();
  }
}

void LEDDriver::begin() {
  if (initialized) {
    Serial.println("LEDDriver: Already initialized");
    return;
  }
  
  Serial.println("LEDDriver: Initializing 4 PWM channels...");
  
  // Configure LEDC timer
  ledc_timer_config_t ledc_timer;
  ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
  ledc_timer.duty_resolution = LEDC_TIMER_8_BIT;  // 8-bit resolution (0-255)
  ledc_timer.timer_num = LEDC_TIMER_0;
  ledc_timer.freq_hz = PWM_FREQUENCY;  // 5kHz
  ledc_timer.clk_cfg = LEDC_AUTO_CLK;
  
  if (ledc_timer_config(&ledc_timer) != ESP_OK) {
    Serial.println("LEDDriver: ERROR - Failed to configure LEDC timer!");
    return;
  }
  
  // Setup each channel
  for (uint8_t i = 0; i < NUM_MOSFET_CHANNELS; i++) {
    setupLEDCChannel(i, mosfetPins[i]);
    Serial.printf("LEDDriver: Channel %d configured on GPIO %d\n", i, mosfetPins[i]);
  }
  
  // Initialize all channels to 0
  clear();
  
  initialized = true;
  Serial.println("LEDDriver: Initialization complete");
}

void LEDDriver::setupLEDCChannel(uint8_t channel, uint8_t pin) {
  if (channel >= NUM_MOSFET_CHANNELS) {
    Serial.printf("LEDDriver: ERROR - Invalid channel %d\n", channel);
    return;
  }
  
  ledc_channel_config_t ledc_channel;
  ledc_channel.gpio_num = pin;
  ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
  ledc_channel.channel = ledcChannels[channel];
  ledc_channel.timer_sel = LEDC_TIMER_0;
  ledc_channel.duty = 0;  // Start with 0 duty cycle
  ledc_channel.hpoint = 0;
  ledc_channel.flags.output_invert = 0;
  
  if (ledc_channel_config(&ledc_channel) != ESP_OK) {
    Serial.printf("LEDDriver: ERROR - Failed to configure channel %d on GPIO %d!\n", channel, pin);
  }
}

void LEDDriver::setChannel(uint8_t channel, uint8_t intensity) {
  if (!initialized) {
    Serial.println("LEDDriver: ERROR - Not initialized! Call begin() first.");
    return;
  }
  
  if (channel >= NUM_MOSFET_CHANNELS) {
    Serial.printf("LEDDriver: ERROR - Invalid channel %d (must be 0-3)\n", channel);
    return;
  }
  
  // Clamp intensity to valid range
  if (intensity > 255) {
    intensity = 255;
  }
  
  channelIntensities[channel] = intensity;
  
  // Update LEDC duty cycle
  // LEDC uses duty cycle from 0 to (2^resolution - 1)
  // For 8-bit: 0-255 maps to 0-255
  uint32_t duty = intensity;
  
  if (ledc_set_duty(LEDC_LOW_SPEED_MODE, ledcChannels[channel], duty) != ESP_OK) {
    Serial.printf("LEDDriver: ERROR - Failed to set duty for channel %d\n", channel);
    return;
  }
  
  // Update duty cycle (required for changes to take effect)
  ledc_update_duty(LEDC_LOW_SPEED_MODE, ledcChannels[channel]);
}

void LEDDriver::setAllChannels(uint8_t intensities[4]) {
  if (!initialized) {
    Serial.println("LEDDriver: ERROR - Not initialized! Call begin() first.");
    return;
  }
  
  for (uint8_t i = 0; i < NUM_MOSFET_CHANNELS; i++) {
    setChannel(i, intensities[i]);
  }
}

uint8_t LEDDriver::getChannelIntensity(uint8_t channel) {
  if (channel >= NUM_MOSFET_CHANNELS) {
    return 0;
  }
  
  return channelIntensities[channel];
}

void LEDDriver::update() {
  // ESP32 LEDC updates automatically when setChannel is called
  // This method is kept for compatibility and potential future use
  // (e.g., batch updates or verification)
  if (!initialized) {
    return;
  }
  
  // Force update all channels (though they should already be updated)
  for (uint8_t i = 0; i < NUM_MOSFET_CHANNELS; i++) {
    ledc_update_duty(LEDC_LOW_SPEED_MODE, ledcChannels[i]);
  }
}

void LEDDriver::clear() {
  if (!initialized) {
    return;
  }
  
  for (uint8_t i = 0; i < NUM_MOSFET_CHANNELS; i++) {
    setChannel(i, 0);
  }
  
  Serial.println("LEDDriver: All channels cleared");
}

