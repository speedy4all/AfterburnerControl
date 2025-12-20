#ifndef CONSTANTS_H
#define CONSTANTS_H

// BLE Device Configuration
#define DEVICE_NAME "ABurner"
#define SERVICE_UUID "b5f9a000-2b6c-4f6a-93b1-2f1f5f9ab000"

// Throttle calibration UUIDs
#define THROTTLE_CALIBRATION_UUID "b5f9a010-2b6c-4f6a-93b1-2f1f5f9ab010"
#define THROTTLE_CALIBRATION_STATUS_UUID "b5f9a011-2b6c-4f6a-93b1-2f1f5f9ab011"
#define THROTTLE_CALIBRATION_RESET_UUID "b5f9a012-2b6c-4f6a-93b1-2f1f5f9ab012"

// Hardware version UUID (optional, for auto-detection)
#define HARDWARE_VERSION_UUID "b5f9a013-2b6c-4f6a-93b1-2f1f5f9ab013"

// Pin definitions - centralized for consistency
#define ONBOARD_LED_PIN 4      // GPIO4 for onboard LED (avoid TX conflict)
#define THROTTLE_PIN 1         // GPIO1 for throttle input

// MOSFET PWM pins for 4-channel LED control
#define MOSFET_1_PIN 2        // GPIO 2 - Channel 0 (controls 9 LEDs)
#define MOSFET_2_PIN 3         // GPIO 3 - Channel 1 (controls 9 LEDs)
#define MOSFET_3_PIN 4         // GPIO 4 - Channel 2 (controls 9 LEDs)
#define MOSFET_4_PIN 5         // GPIO 5 - Channel 3 (controls 9 LEDs)

// LED configuration
#define NUM_MOSFET_CHANNELS 4
#define NUM_LEDS_PER_CHANNEL 9
#define TOTAL_LEDS 36

// PWM configuration
#define PWM_FREQUENCY 5000     // 5kHz PWM frequency (good for LED dimming)
#define PWM_RESOLUTION 8       // 8-bit resolution (0-255)

// Circular effect parameters
#define CIRCLE_ROTATION_SPEED 0.001f  // Rotation speed multiplier

// Timing constants
#define INITIAL_DELAY_MS 1000
#define STATUS_UPDATE_INTERVAL_MS 2000
#define LOOP_DELAY_MS 10
#define LED_TEST_DELAY_MS 500

// PWM timeout for throttle reading
#define PWM_TIMEOUT 25000      // 25ms timeout for PWM read

// Throttle calibration constants
#define DEFAULT_THROTTLE_MIN 900    // Default min throttle PWM (microseconds)
#define DEFAULT_THROTTLE_MAX 2000   // Default max throttle PWM (microseconds)
#define MIN_PWM_VALUE 500           // Minimum valid PWM value
#define MAX_PWM_VALUE 2500          // Maximum valid PWM value
#define CALIBRATION_SAMPLES 10      // Number of samples to average during calibration
#define CALIBRATION_TIMEOUT 30000   // 30 second timeout for calibration

// Enhanced calibration requirements
#define MIN_VISITS_REQUIRED 3       // Minimum visits to min position before completion
#define MAX_VISITS_REQUIRED 3       // Minimum visits to max position before completion
#define MIN_STABILITY_THRESHOLD 50  // PWM difference threshold for considering min/max stable
#define TIME_BETWEEN_VISITS 1000    // Minimum time between visits to same position (ms)

// Serial communication
#define SERIAL_BAUD_RATE 115200

#endif // CONSTANTS_H
