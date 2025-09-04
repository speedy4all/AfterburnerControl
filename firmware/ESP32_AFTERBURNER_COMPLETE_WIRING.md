# ESP32-C3 OLED Development Board Afterburner - Complete Wiring Diagram

This document provides a comprehensive wiring guide for the ESP32-C3 OLED Development Board Afterburner project, including all components and their connections.

## Project Overview

The ESP32-C3 OLED Development Board Afterburner project consists of:

- **ESP32-C3 OLED Development Board** (main controller with built-in OLED)
- **0.42-inch OLED Display** (built-in, shows status and settings)
- **Navigation Button** (manual page control)
- **Throttle Input** (PWM signal from RC receiver or potentiometer)
- **WS2812B LED Strip** (afterburner effect display)
- **Power Supply** (3.3V and 5V)

## ESP32-C3 OLED Development Board Pinout

The ESP32-C3 OLED Development Board is a compact development board with a built-in 0.42-inch OLED display and the following pin layout:

```
ESP32-C3 OLED Development Board
┌─────────────────────────────────────────────────────────┐
│                                                         │
│  ┌─────────────────┐                                    │
│  │  0.42" OLED    │ ← Built-in display (I2C)           │
│  │  128x64 pixels │   SCL: GPIO6, SDA: GPIO5           │
│  │  Built-in      │                                    │
│  └─────────────────┘                                    │
│                                                         │
│  ┌─────────────────┐                                    │
│  │   Navigation    │ ← GPIO2 (ADC2)                     │
│  │     Button      │                                    │
│  └─────────────────┘                                    │
│                                                         │
│  ┌─────────────────┐                                    │
│  │     BOOT        │ ← GPIO0 (ADC0, programming button) │
│  │    Button       │                                    │
│  └─────────────────┘                                    │
│                                                         │
│  ┌─────────────────┐                                    │
│  │     RST         │ ← Reset button                     │
│  │    Button       │                                    │
│  └─────────────────┘                                    │
│                                                         │
│  ┌─────────────────┐                                    │
│  │   USB-C Port    │ ← Programming and power            │
│  └─────────────────┘                                    │
│                                                         │
│  ┌─────────────────┐                                    │
│  │   Ceramic       │ ← WiFi/BLE antenna                │
│  │   Antenna       │                                    │
│  └─────────────────┘                                    │
│                                                         │
└─────────────────────────────────────────────────────────┘

Left Side Pin Headers (from top to bottom):
┌─────────┐
│  5V     │ ← 5V power input/output
│  GND    │ ← Ground
│  3V3    │ ← 3.3V power input/output
│  RX     │ ← GPIO20 (UART RX)
│  TX     │ ← GPIO21 (UART TX)
│  A2     │ ← GPIO2 (ADC2) - Navigation Button
│  A1     │ ← GPIO1 (ADC1) - Throttle Input
│  A0     │ ← GPIO0 (ADC0) - BOOT button
└─────────┘

Right Side Pin Headers (from top to bottom):
┌─────────┐
│  GPIO10 │ ← Available for future use
│  GPIO9  │ ← SCL (I2C Clock) - Available
│  GPIO8  │ ← SDA (I2C Data) - Available
│  GPIO7  │ ← SS (SPI) - LED Data
│  GPIO6  │ ← MOSI (SPI) - OLED SCL
│  GPIO5  │ ← MISO (SPI) - OLED SDA
│  GPIO4  │ ← A4 (ADC4) - Available
│  GPIO3  │ ← A3 (ADC3) - Available
└─────────┘
```

## Component Connections

### 1. OLED Display (Built-in)

```
The 0.42 inch OLED is built into the board and uses:
OLED SCL ──── GPIO6 (I2C Clock)
OLED SDA ──── GPIO5 (I2C Data)
OLED VCC ──── 3.3V (Internal)
OLED GND ──── GND (Internal)

Note: No external connections needed - display is integrated
```

### 2. Navigation Button

```
External Button:
Button Pin 1 ──── GPIO2 (A2 pin)
Button Pin 2 ──── GND
```

### 3. Throttle Input (PWM Signal)

```
RC Receiver or Potentiometer:
Signal Wire ──── GPIO1 (A1 pin)
Power (if needed) ──── 3.3V or 5V
Ground ──── GND
```

### 4. WS2812B LED Strip

```
LED Strip:
Data In ──── GPIO21
VCC ──── 5V (from 5V pin or external 5V supply)
GND ──── GND
```

### 5. Power Supply

```
Power Connections:
USB-C ──── 5V (for programming and power)
5V Pin ──── 5V (for LED strip)
3V3 Pin ──── 3.3V (for sensors if needed)
GND ──── Ground (common)
```

## Updated Pin Assignments for ESP32-C3 OLED Board

Based on the actual board pinout, here are the correct pin assignments:

| Pin    | Function    | Direction     | Notes                |
| ------ | ----------- | ------------- | -------------------- |
| GPIO0  | BOOT Button | Input         | Programming mode     |
| GPIO1  | ADC1        | Input         | Throttle input       |
| GPIO2  | ADC2        | Input         | Navigation button    |
| GPIO3  | ADC3        | Input         | Available            |
| GPIO4  | ADC4        | Input         | Available            |
| GPIO5  | I2C SDA     | Bidirectional | OLED display         |
| GPIO6  | I2C SCL     | Output        | OLED display         |
| GPIO7  | SS (SPI)    | Output        | Available            |
| GPIO8  | SDA (I2C)   | Bidirectional | Available            |
| GPIO9  | SCL (I2C)   | Output        | Available            |
| GPIO10 | Available   | -             | Available            |
| GPIO20 | UART RX     | Input         | Serial communication |
| GPIO21 | UART TX     | Output        | LED data             |

## Detailed Wiring Guide

### Step 1: Navigation Button

```
1. Connect one terminal of momentary push button to GPIO2 (A2 pin)
2. Connect other terminal to GND
3. Internal pull-up resistor keeps pin HIGH when not pressed
4. Button press connects GPIO2 to GND (LOW)
```

### Step 2: Throttle Input

```
Option A - RC Receiver:
1. Connect receiver signal wire to GPIO1 (A1 pin)
2. Connect receiver power to 3.3V or 5V (check receiver specs)
3. Connect receiver ground to GND

Option B - Potentiometer:
1. Connect potentiometer wiper to GPIO1 (A1 pin)
2. Connect one end to 3.3V
3. Connect other end to GND
```

### Step 3: LED Strip

```
1. Connect LED strip data line to GPIO21
2. Connect LED strip VCC to 5V (5V pin or external supply)
3. Connect LED strip GND to GND
4. Ensure adequate power supply for LED count
```

### Step 4: Power Considerations

```
For LED strips with many LEDs:
1. Use external 5V power supply for LED strip
2. Connect LED strip VCC to external 5V
3. Connect LED strip GND to ESP32 GND
4. Keep data line connected to GPIO21
```

## ESP32-C3 OLED Board Specific Notes

### Built-in OLED Display

The board features a built-in 0.42 inch OLED display:

- **Resolution**: 128x64 pixels
- **Interface**: I2C (internal)
- **Pins**: GPIO6 (SCL), GPIO5 (SDA)
- **Power**: 3.3V (internal)
- **No external connections required**

### Pin Availability

The ESP32-C3 OLED board has limited GPIO pins:

- **GPIO0-GPIO2**: System and ADC functions
- **GPIO3-GPIO4**: Available ADC inputs
- **GPIO5-GPIO6**: Built-in OLED (not available for external use)
- **GPIO21**: LED data output
- **GPIO8-GPIO10**: Available for future use
- **GPIO20-GPIO21**: UART communication

### I2C Configuration

The board has one I2C interface:

- **Internal I2C**: GPIO5/6 for built-in OLED
- **External I2C**: GPIO8/9 available for additional devices

### Power Management

The board provides:

- **USB-C**: 5V input for programming and power
- **5V Pin**: 5V output (max ~500mA)
- **3V3 Pin**: 3.3V regulated output (max ~300mA)

## Testing Procedure

### 1. Basic Functionality

```
1. Upload code to ESP32-C3 OLED board
2. Open Serial Monitor (115200 baud)
3. Check for startup messages
4. Verify built-in OLED shows startup screen
```

### 2. Button Testing

```
1. Press navigation button on GPIO2
2. Check Serial Monitor for button press messages
3. Verify OLED pages advance
4. Test button debounce (rapid pressing)
```

### 3. Throttle Testing

```
1. Connect throttle input to GPIO1 (A1 pin)
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

1. **OLED not working**: Built-in display should work automatically
2. **Button not responding**: Verify wiring and pin configuration on GPIO2
3. **LEDs not lighting**: Check power supply and data connection to GPIO7
4. **Throttle not reading**: Verify PWM signal and pin connection to GPIO1
5. **BLE not connecting**: Check antenna and power supply

### ESP32-C3 OLED Board Specific Issues

1. **Built-in OLED**: Should work without any external connections
2. **Limited GPIO pins**: Plan pin usage carefully
3. **Power limitations**: Use external power for high-current components
4. **Upload issues**: Hold BOOT button during upload

### Debug Information

- Serial Monitor shows all system status
- Built-in OLED displays current settings and status
- BLE app provides remote control and monitoring
- LED patterns indicate system state
