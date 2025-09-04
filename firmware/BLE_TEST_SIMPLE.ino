/*
 * Simple ESP32 BLE Test
 * This file tests if the ESP32 BLE library is working correctly
 * Upload this to your ESP32-C3 to test BLE functionality
 */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Test BLE functionality
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 BLE Library Test Starting...");
  
  // Test if we can create BLE objects
  try {
    BLEDevice::init("ESP32-BLE-Test");
    Serial.println("✅ BLEDevice::init() - SUCCESS");
    
    BLEServer* pServer = BLEDevice::createServer();
    Serial.println("✅ BLEServer::createServer() - SUCCESS");
    
    BLEService* pService = pServer->createService("12345678-1234-1234-1234-123456789abc");
    Serial.println("✅ BLEService::createService() - SUCCESS");
    
    BLECharacteristic* pCharacteristic = pService->createCharacteristic(
      "87654321-4321-4321-4321-cba987654321",
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    Serial.println("✅ BLECharacteristic::createCharacteristic() - SUCCESS");
    
    // Test characteristic methods
    pCharacteristic->setValue("Hello BLE!");
    Serial.println("✅ BLECharacteristic::setValue() - SUCCESS");
    
    // ESP32 BLE returns Arduino String, not std::string
    String value = pCharacteristic->getValue();
    Serial.printf("✅ BLECharacteristic::getValue() - SUCCESS, Value: %s\n", value.c_str());
    
    Serial.println("\n🎉 ALL ESP32 BLE TESTS PASSED!");
    Serial.println("The ESP32 BLE library is working correctly.");
    
  } catch (...) {
    Serial.println("❌ ESP32 BLE test FAILED!");
    Serial.println("Check your library installation.");
  }
}

void loop() {
  delay(1000);
  Serial.println("ESP32 BLE Test Running...");
}
