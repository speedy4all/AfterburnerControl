#!/bin/bash

echo "========================================"
echo "ESP8266 Afterburner Controller Flasher"
echo "========================================"
echo

echo "This script will help you flash the Afterburner firmware to your ESP8266."
echo
echo "Requirements:"
echo "- ESP8266 NodeMCU V3 connected via USB"
echo "- Python with esptool installed (pip install esptool)"
echo

# Find available ports
echo "Available serial ports:"
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    ls /dev/cu.usbserial-* 2>/dev/null || echo "No USB serial ports found"
    echo
    read -p "Enter your ESP8266 port (e.g., /dev/cu.usbserial-1410): " PORT
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null || echo "No USB serial ports found"
    echo
    read -p "Enter your ESP8266 port (e.g., /dev/ttyUSB0): " PORT
else
    echo "Unsupported operating system"
    exit 1
fi

if [ -z "$PORT" ]; then
    echo "Error: No port specified!"
    exit 1
fi

echo
echo "Flashing firmware to $PORT..."
echo "This may take 30-60 seconds..."
echo

# Try flashing with high baud rate first
esptool.py --port "$PORT" --baud 921600 write_flash 0x00000 Afterburner_RC_V8266_v2.1.0.bin

if [ $? -ne 0 ]; then
    echo
    echo "Flashing failed! Trying with slower baud rate..."
    esptool.py --port "$PORT" --baud 115200 write_flash 0x00000 Afterburner_RC_V8266_v2.1.0.bin
    
    if [ $? -ne 0 ]; then
        echo
        echo "Flashing failed! Please check:"
        echo "1. ESP8266 is connected and in flash mode"
        echo "2. Correct port is selected"
        echo "3. esptool is installed (pip install esptool)"
        echo "4. Try holding FLASH button while pressing RESET"
        echo "5. Check permissions: sudo chmod 666 $PORT"
        exit 1
    fi
fi

echo
echo "========================================"
echo "Flashing completed successfully!"
echo "========================================"
echo
echo "Next steps:"
echo "1. Disconnect and reconnect your ESP8266"
echo "2. Open serial monitor at 115200 baud"
echo "3. You should see calibration messages"
echo "4. Connect to 'Afterburner_AP' WiFi network"
echo "5. Download the mobile app and start controlling!"
echo
