@echo off
echo ========================================
echo ESP8266 Afterburner Controller Flasher
echo ========================================
echo.

echo This script will help you flash the Afterburner firmware to your ESP8266.
echo.
echo Requirements:
echo - ESP8266 NodeMCU V3 connected via USB
echo - Python with esptool installed (pip install esptool)
echo.

set /p PORT="Enter your ESP8266 COM port (e.g., COM3): "

if "%PORT%"=="" (
    echo Error: No COM port specified!
    pause
    exit /b 1
)

echo.
echo Flashing firmware to %PORT%...
echo This may take 30-60 seconds...
echo.

esptool.py --port %PORT% --baud 921600 write_flash 0x00000 Afterburner_RC_V8266_v2.1.0.bin

if %errorlevel% neq 0 (
    echo.
    echo Flashing failed! Trying with slower baud rate...
    esptool.py --port %PORT% --baud 115200 write_flash 0x00000 Afterburner_RC_V8266_v2.1.0.bin
    
    if %errorlevel% neq 0 (
        echo.
        echo Flashing failed! Please check:
        echo 1. ESP8266 is connected and in flash mode
        echo 2. Correct COM port is selected
        echo 3. esptool is installed (pip install esptool)
        echo 4. Try holding FLASH button while pressing RESET
        pause
        exit /b 1
    )
)

echo.
echo ========================================
echo Flashing completed successfully!
echo ========================================
echo.
echo Next steps:
echo 1. Disconnect and reconnect your ESP8266
echo 2. Open serial monitor at 115200 baud
echo 3. You should see calibration messages
echo 4. Connect to "Afterburner_AP" WiFi network
echo 5. Download the mobile app and start controlling!
echo.
pause
