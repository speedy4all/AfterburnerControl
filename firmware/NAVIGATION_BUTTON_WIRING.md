# ESP32 C3 Navigation Button Wiring Guide

This guide explains how to add a dedicated navigation button for the OLED display on the ESP32 C3 Afterburner project.

## Button Specifications

- **Type**: Momentary push button (normally open)
- **Voltage**: 3.3V (same as ESP32 C3 logic level)
- **Current**: Minimal (few mA)

## Wiring Diagram

```
ESP32 C3 Pinout:
┌─────────────────┐
│                 │
│  GPIO2 ──────── │ ← Navigation Button
│                 │
│  GPIO4 ──────── │ ← I2C SDA (OLED)
│                 │
│  GPIO5 ──────── │ ← I2C SCL (OLED)
│                 │
│  3.3V ───────── │ ← Power
│                 │
│  GND ────────── │ ← Ground
│                 │
└─────────────────┘
```

## Connection Details

### Navigation Button (GPIO2)

```
Button Pin 1 ──── GPIO2 (with internal pull-up)
Button Pin 2 ──── GND
```

### Alternative GPIO Pins

If GPIO2 is not available, you can use any of these GPIO pins:

- **GPIO1**: Available (but used for UART TX)
- **GPIO3**: Available (but used for UART RX)
- **GPIO6**: Available
- **GPIO7**: Available
- **GPIO8**: Available
- **GPIO9**: Available
- **GPIO10**: Available

**Avoid these pins:**

- **GPIO0**: BOOT button (used for programming)
- **GPIO4**: I2C SDA (used for OLED)
- **GPIO5**: I2C SCL (used for OLED)

## Hardware Setup

### Option 1: External Button

1. Connect one terminal of the button to GPIO2
2. Connect the other terminal to GND
3. The internal pull-up resistor will keep the pin HIGH when not pressed
4. When pressed, the button connects GPIO2 to GND, making it LOW

### Option 2: Using Existing BOOT Button

If you prefer to use the existing BOOT button:

1. Change the pin in `main.cpp` from `oledDisplay.begin(2)` to `oledDisplay.begin(0)`
2. Note: This will interfere with programming mode when held during boot

### Option 3: No Button (Auto-advance)

If you don't want to add a button:

1. Change the pin in `main.cpp` to `oledDisplay.begin(0)` (or any unused pin)
2. The display will automatically advance pages every 3 seconds

## Button Behavior

- **Single Press**: Advances to next page
- **Debounce**: 200ms to prevent multiple triggers
- **Feedback**: Serial output shows page changes
- **Auto-advance**: Disabled when button is configured

## Testing

1. Upload the code to your ESP32 C3
2. Open Serial Monitor (115200 baud)
3. Press the navigation button
4. You should see: "Navigation button pressed - Page X/3"
5. The OLED display should advance to the next page

## Troubleshooting

### Button Not Working

1. Check wiring connections
2. Verify button is connected to correct GPIO pin
3. Check Serial Monitor for initialization message
4. Ensure button is momentary (not latching)

### Multiple Triggers

1. The code includes 200ms debounce
2. If still having issues, increase `buttonDebounceTime` in the code

### Wrong GPIO Pin

1. Change the pin number in `main.cpp`: `oledDisplay.begin(PIN_NUMBER)`
2. Re-upload the code

## Code Configuration

To change the button pin, modify this line in `main.cpp`:

```cpp
oledDisplay.begin(2); // Change 2 to your desired GPIO pin
```

To disable the button and use auto-advance:

```cpp
oledDisplay.begin(0); // Uses GPIO0 (BOOT button) or auto-advance
```
