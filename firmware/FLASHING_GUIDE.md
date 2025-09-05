# ESP8266 Afterburner Controller - Binary Flashing Guide

This guide shows you how to flash the pre-compiled binary file to your ESP8266 NodeMCU V3 without needing to compile the source code.

## 📦 What You Need

### Hardware

- ESP8266 NodeMCU V3 (or compatible board)
- USB cable (Micro-USB)
- Computer with USB port

### Software

Choose one of these flashing tools:

#### Option 1: ESP8266 Flash Download Tool (Recommended)

- **Download**: [ESP8266 Flash Download Tool](https://www.espressif.com/en/support/download/other-tools)
- **Platform**: Windows
- **Advantages**: Official tool, reliable, supports all ESP8266 variants

#### Option 2: esptool.py (Cross-platform)

- **Install**: `pip install esptool`
- **Platform**: Windows, macOS, Linux
- **Advantages**: Command-line, scriptable, works everywhere

#### Option 3: Arduino IDE (Alternative)

- **Download**: [Arduino IDE](https://www.arduino.cc/en/software)
- **Platform**: Windows, macOS, Linux
- **Advantages**: Familiar interface, built-in ESP8266 support

## 🚀 Flashing Methods

### Method 1: ESP8266 Flash Download Tool (Windows)

1. **Download and Install**

   - Download ESP8266 Flash Download Tool from Espressif
   - Extract and run `flash_download_tool.exe`

2. **Configure Settings**

   ```
   SPI SPEED: 40MHz
   SPI MODE: DIO
   FLASH SIZE: 4MB (32Mbit)
   COM PORT: Select your ESP8266's COM port
   BAUD: 921600 (or 115200 if 921600 fails)
   ```

3. **Load Binary File**

   - Click "..." next to "0x00000"
   - Select `Afterburner_RC_V8266_v2.1.0.bin`
   - Set address to `0x00000`

4. **Flash the Firmware**
   - Click "START" button
   - Wait for completion (usually 30-60 seconds)
   - You should see "FINISH" message

### Method 2: esptool.py (Command Line)

1. **Install esptool**

   ```bash
   pip install esptool
   ```

2. **Find Your ESP8266 Port**

   - **Windows**: Check Device Manager for COM port
   - **macOS**: Run `ls /dev/cu.*` and look for `cu.usbserial-*`
   - **Linux**: Run `ls /dev/ttyUSB*` or `ls /dev/ttyACM*`

3. **Flash the Binary**

   ```bash
   esptool.py --port COM3 --baud 921600 write_flash 0x00000 Afterburner_RC_V8266_v2.1.0.bin
   ```

   Replace `COM3` with your actual port.

4. **Verify Flash**
   ```bash
   esptool.py --port COM3 verify_flash 0x00000 Afterburner_RC_V8266_v2.1.0.bin
   ```

### Method 3: Arduino IDE

1. **Install ESP8266 Board Package**

   - Open Arduino IDE
   - Go to File → Preferences
   - Add this URL to "Additional Board Manager URLs":
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Go to Tools → Board → Boards Manager
   - Search for "ESP8266" and install

2. **Configure Board Settings**

   - Select Tools → Board → NodeMCU 1.0 (ESP-12E Module)
   - Select your COM port
   - Set Upload Speed to 921600

3. **Flash Using Arduino IDE**
   - Open any Arduino sketch (or create a blank one)
   - Go to Sketch → Upload Using Programmer
   - Or use the "Upload" button (this will compile and upload)

## 🔧 Troubleshooting

### Common Issues

#### "Failed to connect to ESP8266"

**Solutions:**

- Press and hold the RESET button, then press and hold the FLASH button, release RESET, then release FLASH
- Try different baud rates (115200, 460800, 921600)
- Check USB cable (use a data cable, not just power)
- Try different USB port

#### "Flash download failed"

**Solutions:**

- Ensure ESP8266 is in flash mode (hold FLASH button while pressing RESET)
- Try slower baud rate (115200)
- Check that the binary file is not corrupted
- Ensure sufficient power supply

#### "Wrong flash size"

**Solutions:**

- Set flash size to 4MB (32Mbit) in flashing tool
- Some boards may need 1MB (8Mbit) - try both

#### "Permission denied" (Linux/macOS)

**Solutions:**

```bash
sudo chmod 666 /dev/ttyUSB0  # Replace with your port
# Or add your user to dialout group:
sudo usermod -a -G dialout $USER
```

### Verification Steps

After flashing, verify the installation:

1. **Connect to Serial Monitor**

   - Open serial monitor at 115200 baud
   - You should see:
     ```
     ESP8266 Afterburner Starting...
     === THROTTLE CALIBRATION STARTED ===
     Move your throttle stick from MIN to MAX several times
     ```

2. **Check WiFi Network**

   - Look for "Afterburner_AP" WiFi network
   - Password: "afterburner123"

3. **Test Mobile App**
   - Download the Afterburner Control app
   - Connect to "Afterburner_AP" network
   - Open app and test connection

## 📋 Binary File Information

- **File**: `Afterburner_RC_V8266_v2.1.0.bin`
- **Size**: ~352 KB
- **Flash Address**: 0x00000
- **Target Board**: ESP8266 NodeMCU V3
- **Version**: 2.1.0
- **Features**: Automatic calibration, crash prevention, WebSocket control

## 🎯 Next Steps

After successful flashing:

1. **Hardware Setup**

   - Connect throttle wire to D5 (GPIO14)
   - Connect LED strip data wire to D6 (GPIO12)
   - Connect 5V power supply for LED strip
   - Ensure common ground connection

2. **Calibration**

   - Power on the ESP8266
   - Move throttle stick from MIN to MAX several times
   - Wait for calibration to complete (~5-10 seconds)

3. **Mobile App Setup**
   - Connect to "Afterburner_AP" WiFi network
   - Download and open the Afterburner Control app
   - Start controlling your LED effects!

## 📞 Support

If you encounter issues:

1. Check the troubleshooting section above
2. Verify your hardware connections
3. Try reflashing with a different method
4. Check the main project documentation
5. Open an issue on GitHub with detailed error information

---

**Your ESP8266 Afterburner Controller is now ready to use!** 🚀
