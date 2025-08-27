# Testing Guide for ESP32 Afterburner Firmware

This document explains how to run tests for the ESP32 Afterburner firmware.

## Test Environment

### ESP32 Test Environment (`esp32dev_test`)

This environment runs tests directly on the ESP32 hardware. It's the most reliable way to test the firmware since it uses the actual hardware.

#### How to Run ESP32 Tests

1. **Build the test firmware:**

   ```bash
   pio run -e esp32dev_test
   ```

2. **Upload to ESP32:**

   ```bash
   pio run -e esp32dev_test --target upload
   ```

3. **Monitor test results:**
   ```bash
   pio device monitor
   ```

#### What the Tests Do

The ESP32 test environment:

- Initializes all components (settings, LED effects, throttle, BLE, OLED)
- Verifies each component initializes correctly
- Reports test results via serial output
- Automatically reboots after tests complete

#### Test Output Example

```
ESP32 Afterburner Starting...
=== RUNNING TESTS ===
Test 1: Settings initialization
Default mode: 0
Default brightness: 128
Default numLeds: 60
✓ Settings test passed
Test 2: LED effects initialization
✓ LED effects test passed
Test 3: Throttle reading
Throttle value: 0.00
✓ Throttle test passed
Test 4: BLE service initialization
✓ BLE service test passed
Test 5: OLED display initialization
✓ OLED display test passed
=== ALL TESTS PASSED ===
Test mode complete. Rebooting in 5 seconds...
```

## GitHub Actions Testing

### Automated CI/CD Testing

The project includes optimized GitHub Actions workflows that automatically test the firmware:

#### Workflow Structure

- **`firmware-test.yml`**: Dedicated firmware testing (runs when `firmware/**` changes)
- **`test.yml`**: React Native application testing
- **`react-native-build.yml`**: React Native app building

#### What GitHub Actions Tests

1. **Build Verification**: Ensures both normal and test firmware compile successfully
2. **Size Analysis**: Checks that firmware sizes are reasonable
3. **Artifact Upload**: Saves compiled firmware for download
4. **Test Report Generation**: Creates a summary of test coverage
5. **Caching**: PlatformIO dependencies cached for faster builds

#### What Gets Tested in CI

✅ **Build Success**: Both `esp32dev` and `esp32dev_test` environments compile  
✅ **Firmware Size**: Checks that firmware is under reasonable size limits  
✅ **Test Code Integration**: Verifies test mode compiles with all components  
✅ **Dependency Resolution**: Ensures all libraries are properly linked

#### Limitations of CI Testing

❌ **No Hardware Testing**: GitHub Actions cannot run actual ESP32 hardware tests  
❌ **No Serial Output**: Cannot monitor actual test execution  
❌ **No BLE Testing**: Cannot test Bluetooth functionality

#### How to Interpret CI Results

- **✅ Green Check**: Firmware builds successfully, ready for hardware testing
- **❌ Red X**: Build failed, needs code fixes before hardware testing
- **⚠️ Warning**: Firmware size issues or other concerns

#### Next Steps After CI Passes

1. Download the test firmware from GitHub Actions artifacts
2. Upload to your ESP32 hardware
3. Monitor serial output for actual test results

## Quick Test Commands

### Windows (Batch Files)

**Run ESP32 tests:**

```cmd
.\run_tests_simple.bat
```

### Manual Commands

1. **Build and test:**

   ```bash
   pio run -e esp32dev_test
   pio run -e esp32dev_test --target upload
   pio device monitor
   ```

2. **Build only:**
   ```bash
   pio run -e esp32dev
   ```

## Test Files

- `src/main.cpp` - Contains the test mode implementation
- `platformio.ini` - Contains test environment configurations
- `.github/workflows/` - GitHub Actions workflows

## Troubleshooting

### Build Failures

1. Make sure all dependencies are installed:

   ```bash
   pio pkg install
   ```

2. Check that your ESP32 is connected and recognized:

   ```bash
   pio device list
   ```

3. Try cleaning and rebuilding:
   ```bash
   pio run -t clean
   pio run -e esp32dev_test
   ```

### GitHub Actions Failures

1. Check the build logs for specific error messages
2. Ensure all dependencies are properly specified in `platformio.ini`
3. Verify that the ESP32 platform and libraries are compatible
4. Check that firmware size is within reasonable limits

## Test Coverage

The current tests cover:

- ✅ Settings initialization and default values
- ✅ LED effects initialization
- ✅ Throttle reading functionality
- ✅ BLE service initialization
- ✅ OLED display initialization

## Adding New Tests

To add new tests:

1. Modify the `runTests()` function in `src/main.cpp`
2. Add your test logic
3. Use `Serial.println()` to report results
4. Rebuild and upload the test firmware

Example:

```cpp
// Test 6: New functionality
Serial.println("Test 6: New functionality");
// Your test code here
Serial.println("✓ New functionality test passed");
```
