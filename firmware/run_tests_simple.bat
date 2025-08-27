@echo off
echo ========================================
echo ESP32 Afterburner Test Runner
echo ========================================

echo Building test firmware...
pio run -e esp32dev_test

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo BUILD SUCCESSFUL!
    echo ========================================
    echo.
    echo To run the tests:
    echo 1. Upload the firmware to your ESP32:
    echo    pio run -e esp32dev_test --target upload
    echo.
    echo 2. Monitor the serial output:
    echo    pio device monitor
    echo.
    echo The firmware will run tests automatically and reboot.
    echo.
    echo Test results will be displayed in the serial monitor.
) else (
    echo.
    echo ========================================
    echo BUILD FAILED!
    echo ========================================
)

pause
