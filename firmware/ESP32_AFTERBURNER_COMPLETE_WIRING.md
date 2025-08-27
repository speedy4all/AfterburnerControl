# ESP32 C3 Afterburner - Complete Wiring Diagram

This document provides a comprehensive wiring guide for the ESP32 C3 Afterburner project, including all components and their connections.

## Project Overview

The ESP32 C3 Afterburner project consists of:

- **ESP32 C3 OLED Development Board** (main controller)
- **0.42-inch OLED Display** (built-in, shows status and settings)
- **Navigation Button** (manual page control)
- **Throttle Input** (PWM signal from RC receiver or potentiometer)
- **WS2812B LED Strip** (afterburner effect display)
- **Power Supply** (3.3V and 5V)

## Complete Pinout Diagram

```
ESP32 C3 OLED Development Board
┌─────────────────────────────────────────────────────────┐
│                                                         │
│  ┌─────────────────┐                                    │
│  │  0.42" OLED    │ ← Built-in display (I2C)           │
│  │  128x64 pixels │   SDA: GPIO4, SCL: GPIO5           │
│  │  Starting:13,14│                                    │
│  └─────────────────┘                                    │
│                                                         │
│  ┌─────────────────┐                                    │
│  │   Navigation    │ ← GPIO2 (with pull-up)            │
│  │     Button      │                                    │
│  └─────────────────┘                                    │
│                                                         │
│  ┌─────────────────┐                                    │
│  │     BOOT        │ ← GPIO0 (programming button)      │
│  │    Button       │                                    │
│  └─────────────────┘                                    │
│                                                         │
│  ┌─────────────────┐                                    │
│  │     RST         │ ← Reset button                    │
│  │    Button       │                                    │
│  └─────────────────┘                                    │
│                                                         │
│  ┌─────────────────┐                                    │
│  │   USB-C Port    │ ← Programming and power           │
│  └─────────────────┘                                    │
│                                                         │
│  ┌─────────────────┐                                    │
│  │   Ceramic       │ ← WiFi/BLE antenna                │
│  │   Antenna       │                                    │
│  └─────────────────┘                                    │
│                                                         │
└─────────────────────────────────────────────────────────┘

Pin Headers (Left Side):
┌─────────┐
│  3      │ ← GPIO3 (UART RX)
│  4      │ ← GPIO4 (I2C SDA - OLED)
│  5      │ ← GPIO5 (I2C SCL - OLED)
│  6      │ ← GPIO6 (Available)
│  7      │ ← GPIO7 (Available)
│  8      │ ← GPIO8 (Available)
│  9      │ ← GPIO9 (Available)
│  10     │ ← GPIO10 (Available)
│  PWR    │ ← Power indicator
└─────────┘

Pin Headers (Right Side):
┌─────────┐
│  0      │ ← GPIO0 (BOOT button)
│  1      │ ← GPIO1 (UART TX)
│  2      │ ← GPIO2 (Navigation button)
│  TX     │ ← UART TX (GPIO1)
│  RX     │ ← UART RX (GPIO3)
│  V3     │ ← 3.3V power
│  GD     │ ← Ground
│  V5     │ ← 5V power
└─────────┘
```

## Component Connections

### 1. OLED Display (Built-in)

```
Internal Connections:
OLED SDA ──── GPIO4 (I2C Data)
OLED SCL ──── GPIO5 (I2C Clock)
OLED VCC ──── 3.3V (Internal)
OLED GND ──── GND (Internal)
```

### 2. Navigation Button

```
External Button:
Button Pin 1 ──── GPIO2
Button Pin 2 ──── GND
```

### 3. Throttle Input (PWM Signal)

```
RC Receiver or Potentiometer:
Signal Wire ──── GPIO34 (THR_PIN)
Power (if needed) ──── 3.3V or 5V
Ground ──── GND
```

### 4. WS2812B LED Strip

```
LED Strip:
Data In ──── GPIO18 (LED_PIN)
VCC ──── 5V (from V5 pin or external 5V supply)
GND ──── GND
```

### 5. Power Supply

```
Power Connections:
USB-C ──── 5V (for programming and power)
V5 Pin ──── 5V (for LED strip)
V3 Pin ──── 3.3V (for sensors if needed)
GND ──── Ground (common)
```

## Detailed Wiring Guide

### Step 1: Navigation Button

```
1. Connect one terminal of momentary push button to GPIO2
2. Connect other terminal to GND
3. Internal pull-up resistor keeps pin HIGH when not pressed
4. Button press connects GPIO2 to GND (LOW)
```

### Step 2: Throttle Input

```
Option A - RC Receiver:
1. Connect receiver signal wire to GPIO34
2. Connect receiver power to 3.3V or 5V (check receiver specs)
3. Connect receiver ground to GND

Option B - Potentiometer:
1. Connect potentiometer wiper to GPIO34
2. Connect one end to 3.3V
3. Connect other end to GND
```

### Step 3: LED Strip

```
1. Connect LED strip data line to GPIO18
2. Connect LED strip VCC to 5V (V5 pin or external supply)
3. Connect LED strip GND to GND
4. Ensure adequate power supply for LED count
```

### Step 4: Power Considerations

```
For LED strips with many LEDs:
1. Use external 5V power supply for LED strip
2. Connect LED strip VCC to external 5V
3. Connect LED strip GND to ESP32 GND
4. Keep data line connected to GPIO18
```

## Pin Usage Summary

| Pin    | Function          | Direction     | Notes                |
| ------ | ----------------- | ------------- | -------------------- |
| GPIO0  | BOOT Button       | Input         | Programming mode     |
| GPIO1  | UART TX           | Output        | Serial communication |
| GPIO2  | Navigation Button | Input         | OLED page control    |
| GPIO3  | UART RX           | Input         | Serial communication |
| GPIO4  | I2C SDA           | Bidirectional | OLED display         |
| GPIO5  | I2C SCL           | Output        | OLED display         |
| GPIO6  | Available         | -             | Future use           |
| GPIO7  | Available         | -             | Future use           |
| GPIO8  | Available         | -             | Future use           |
| GPIO9  | Available         | -             | Future use           |
| GPIO10 | Available         | -             | Future use           |
| GPIO18 | LED Data          | Output        | WS2812B strip        |
| GPIO34 | Throttle Input    | Input         | PWM signal           |

## Power Requirements

### ESP32 C3 Board

- **Voltage**: 5V via USB-C or V5 pin
- **Current**: ~100-200mA (without LEDs)
- **Power**: ~0.5-1W

### LED Strip

- **Voltage**: 5V
- **Current**: ~60mA per LED at full brightness
- **Power**: ~0.3W per LED

### Total Power Requirements

- **45 LEDs**: ~2.7A at 5V (13.5W)
- **100 LEDs**: ~6A at 5V (30W)
- **Recommendation**: Use external 5V power supply for LED strips

## Safety Considerations

### Power Supply

1. **Use adequate power supply** for LED count
2. **Add capacitors** near LED strip for stability
3. **Check voltage levels** before connecting
4. **Use proper wire gauge** for current requirements

### Connections

1. **Double-check polarity** before powering on
2. **Secure connections** to prevent shorts
3. **Use heat shrink** or electrical tape for insulation
4. **Test with multimeter** before final assembly

### LED Strip

1. **Cut only at marked points** on LED strip
2. **Check data flow direction** (arrow on strip)
3. **Limit strip length** to prevent voltage drop
4. **Add power injection** for long strips

## Testing Procedure

### 1. Basic Functionality

```
1. Upload code to ESP32 C3
2. Open Serial Monitor (115200 baud)
3. Check for startup messages
4. Verify OLED display shows startup screen
```

### 2. Button Testing

```
1. Press navigation button
2. Check Serial Monitor for button press messages
3. Verify OLED pages advance
4. Test button debounce (rapid pressing)
```

### 3. Throttle Testing

```
1. Connect throttle input
2. Move throttle through full range
3. Check Serial Monitor for throttle values
4. Verify OLED shows throttle percentage
```

### 4. LED Testing

```
1. Power up LED strip
2. Check for startup LED pattern
3. Move throttle to see LED response
4. Test different modes via BLE app
```

### 5. BLE Testing

```
1. Open BLE app on phone
2. Scan for "ABurner" device
3. Connect and test settings changes
4. Verify OLED updates with new settings
```

## Troubleshooting

### Common Issues

1. **OLED not working**: Check I2C connections and power
2. **Button not responding**: Verify wiring and pin configuration
3. **LEDs not lighting**: Check power supply and data connection
4. **Throttle not reading**: Verify PWM signal and pin connection
5. **BLE not connecting**: Check antenna and power supply

### Debug Information

- Serial Monitor shows all system status
- OLED displays current settings and status
- BLE app provides remote control and monitoring
- LED patterns indicate system state
