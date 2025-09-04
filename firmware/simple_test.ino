/*
 * ESP32-C3 SuperMini Simple Test
 * 
 * This is a basic test program for the ESP32-C3 SuperMini board
 * Upload this to Arduino IDE and open the Serial Monitor at 115200 baud
 */

// Pin definitions for ESP32-C3 SuperMini
#define ONBOARD_LED_PIN 8
#define TEST_PIN_1 2
#define TEST_PIN_2 3

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(2000); // Wait for serial to stabilize
  
  Serial.println("========================================");
  Serial.println("ESP32-C3 SuperMini Simple Test");
  Serial.println("========================================");
  Serial.println("Board: ESP32-C3 SuperMini");
  Serial.println("Chip: ESP32-C3 RISC-V @ 160MHz");
  Serial.println("Flash: 4MB");
  Serial.println("SRAM: 400KB");
  Serial.println("========================================");
  
  // Initialize GPIO pins
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  pinMode(TEST_PIN_1, OUTPUT);
  pinMode(TEST_PIN_2, OUTPUT);
  
  Serial.println("GPIO pins initialized");
  Serial.println("Onboard LED: GPIO8");
  Serial.println("Test pins: GPIO2, GPIO3");
  Serial.println("========================================");
  Serial.println("ESP32-C3 SuperMini Ready!");
  Serial.println("========================================");
}

void loop() {
  static unsigned long lastBlink = 0;
  static unsigned long lastTest = 0;
  static bool ledState = false;
  static bool testState = false;
  
  unsigned long currentTime = millis();
  
  // Blink onboard LED every 1 second
  if (currentTime - lastBlink > 1000) {
    ledState = !ledState;
    digitalWrite(ONBOARD_LED_PIN, ledState);
    lastBlink = currentTime;
    
    Serial.printf("Onboard LED: %s\n", ledState ? "ON" : "OFF");
  }
  
  // Toggle test pins every 2 seconds
  if (currentTime - lastTest > 2000) {
    testState = !testState;
    digitalWrite(TEST_PIN_1, testState);
    digitalWrite(TEST_PIN_2, !testState);
    lastTest = currentTime;
    
    Serial.printf("Test pins: GPIO2=%s, GPIO3=%s\n", 
                  testState ? "HIGH" : "LOW", 
                  !testState ? "HIGH" : "LOW");
  }
  
  // Print system info every 5 seconds
  static unsigned long lastInfo = 0;
  if (currentTime - lastInfo > 5000) {
    Serial.printf("System Info - Uptime: %lu seconds, Free Heap: %lu bytes\n", 
                  currentTime / 1000, ESP.getFreeHeap());
    lastInfo = currentTime;
  }
  
  delay(100);
}
