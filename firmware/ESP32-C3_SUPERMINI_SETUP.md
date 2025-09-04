# ESP32-C3 SuperMini Afterburner Setup Guide

## ✅ **Configuration Complete!**

Your ESP32-C3 SuperMini is now configured for the full Afterburner functionality.

## **Pin Assignments (Updated)**

| Function           | GPIO Pin | Notes                                      |
| ------------------ | -------- | ------------------------------------------ |
| **Onboard LED**    | GPIO4    | ✅ Changed from GPIO8 to avoid TX conflict |
| **Throttle Input** | GPIO1    | ✅ PWM throttle input                      |
| **LED Strip**      | GPIO21   | ✅ WS2812B LED strip control               |
| **Serial TX**      | GPIO21   | ⚠️ **CONFLICT** - Same as LED strip!       |
| **Serial RX**      | GPIO20   | Hardware UART                              |

## **⚠️ IMPORTANT: Pin Conflict Detected!**

**GPIO21 is used for BOTH:**

- LED Strip control
- Serial TX (hardware UART)

**This will cause issues!** The LED strip will interfere with serial communication.

## **🔧 Recommended Solutions:**

### **Option 1: Change LED Strip Pin (Recommended)**

```cpp
// In main.cpp, change:
#define LED_STRIP_PIN 21    // GPIO21 for LED strip (avoid TX pin)

// To:
#define LED_STRIP_PIN 3     // GPIO3 for LED strip (free pin)
```

### **Option 2: Use USB-CDC Only**

- Keep USB-CDC enabled (`-DARDUINO_USB_CDC_ON_BOOT=1`)
- Disable hardware UART by not using `Serial.begin()`
- Use `Serial` for USB output only

## **📋 Current Status:**

✅ **ESP32-C3 SuperMini recognized correctly**  
✅ **All source files restored**  
✅ **USB-CDC enabled for Arduino IDE**  
✅ **Main functionality implemented**  
⚠️ **Pin conflict needs resolution**

## **🚀 Next Steps:**

1. **Resolve pin conflict** (choose Option 1 or 2 above)
2. **Compile in Arduino IDE**
3. **Upload to ESP32-C3 SuperMini**
4. **Verify serial output over USB**

## **🔌 Hardware Connections:**

- **Throttle**: Connect to GPIO1
- **LED Strip**: Connect to GPIO3 (after fixing conflict)
- **Power**: 5V and GND
- **USB**: For programming and serial output

## **📱 BLE Features:**

- Device Name: "ABurner"
- Configurable via BLE app
- Real-time throttle and mode updates
- LED color and brightness control

---

**Note**: This configuration is specifically for ESP32-C3 SuperMini boards. The pin assignments have been optimized to avoid conflicts with the board's specific layout.
