#include "wifi_service.h"

// Static instance for WebSocket callback
static AfterburnerWiFiService* wifiServiceInstance = nullptr;

// WebSocket event handler wrapper
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (wifiServiceInstance) {
    wifiServiceInstance->onWebSocketEvent(num, type, payload, length);
  }
}

AfterburnerWiFiService::AfterburnerWiFiService(SettingsManager* settings, ThrottleReader* throttle) {
  settingsManager = settings;
  throttleReader = throttle;
  webServer = nullptr;
  webSocket = nullptr;
  lastStatusUpdate = 0;
  lastSettingsUpdate = 0;
  settingsNeedSave = false;
  clientConnected = false;
  lastThrottle = 0.0;
  wifiServiceInstance = this;
}

void AfterburnerWiFiService::begin() {
  // Setup WiFi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("WiFi AP started. SSID: ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  
  // Setup Web Server
  setupWebServer();
  
  // Setup WebSocket
  setupWebSocket();
  
  Serial.println("WiFi service started");
}

void AfterburnerWiFiService::setupWebServer() {
  webServer = new ESP8266WebServer(WEB_SERVER_PORT);
  
  // Setup routes
  webServer->on("/", std::bind(&AfterburnerWiFiService::handleRoot, this));
  webServer->on("/settings", std::bind(&AfterburnerWiFiService::handleSettings, this));
  webServer->on("/update", HTTP_POST, std::bind(&AfterburnerWiFiService::handleUpdateSettings, this));
  webServer->on("/status", std::bind(&AfterburnerWiFiService::handleStatus, this));
  
  webServer->begin();
  Serial.println("Web server started on port 80");
}

void AfterburnerWiFiService::setupWebSocket() {
  webSocket = new WebSocketsServer(WEB_SOCKET_PORT);
  webSocket->onEvent(webSocketEvent);
  webSocket->begin();
  Serial.printf("WebSocket server started on port %d\n", WEB_SOCKET_PORT);
}

void AfterburnerWiFiService::loop() {
  if (webServer) {
    webServer->handleClient();
  }
  if (webSocket) {
    webSocket->loop();
  }
  
  // Handle deferred settings save
  if (settingsNeedSave) {
    Serial.println("Saving settings to EEPROM in main loop...");
    ESP.wdtFeed();
    
    // Add a small delay to ensure we're not in a critical section
    delay(5);
    
    settingsManager->saveSettings();
    settingsNeedSave = false;
    
    // Add another small delay after saving
    delay(5);
    
    Serial.println("Settings saved successfully");
  }
}

void AfterburnerWiFiService::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  // Feed watchdog at the start of event handling
  ESP.wdtFeed();
  
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      clientConnected = false;
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket->remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
        clientConnected = true;
        
        // Feed watchdog before JSON operations
        ESP.wdtFeed();
        
        // Send current settings to client
        AfterburnerSettings& settings = settingsManager->getSettings();
        StaticJsonDocument<256> doc;
        doc["type"] = "settings";
        doc["mode"] = settings.mode;
        JsonArray startColorArray = doc.createNestedArray("startColor");
        startColorArray.add(settings.startColor[0]);
        startColorArray.add(settings.startColor[1]);
        startColorArray.add(settings.startColor[2]);
        
        JsonArray endColorArray = doc.createNestedArray("endColor");
        endColorArray.add(settings.endColor[0]);
        endColorArray.add(settings.endColor[1]);
        endColorArray.add(settings.endColor[2]);
        doc["speedMs"] = settings.speedMs;
        doc["brightness"] = settings.brightness;
        doc["numLeds"] = settings.numLeds;
        doc["abThreshold"] = settings.abThreshold;
        
        String jsonString;
        serializeJson(doc, jsonString);
        webSocket->sendTXT(num, jsonString);
        
        // Feed watchdog after sending
        ESP.wdtFeed();
      }
      break;
    case WStype_TEXT:
      {
        Serial.printf("WebSocket text received, length: %d\n", length);
        if (length > 0) {
          String message = String((char*)payload);
          Serial.print("WebSocket text received: ");
          Serial.println(message);
          
          // Check if it's a ping message for testing
          if (message == "ping") {
            Serial.println("Ping received, sending pong");
            webSocket->sendTXT(num, "pong");
            return;
          }
          
          // Handle JSON commands
          if (message.startsWith("{")) {
            StaticJsonDocument<128> doc;
            DeserializationError error = deserializeJson(doc, message);
            
            if (!error) {
              const char* type = doc["type"];
                             if (strcmp(type, "command") == 0) {
                 const char* command = doc["command"];
                 if (strcmp(command, "start_calibration") == 0) {
                   Serial.println("Received start_calibration command");
                   throttleReader->startCalibration();
                   webSocket->sendTXT(num, "{\"type\":\"response\",\"command\":\"start_calibration\",\"status\":\"started\"}");
                   return;
                 } else if (strcmp(command, "reset_calibration") == 0) {
                   Serial.println("Received reset_calibration command");
                   throttleReader->resetCalibration();
                   webSocket->sendTXT(num, "{\"type\":\"response\",\"command\":\"reset_calibration\",\"status\":\"completed\"}");
                   return;
                 }
               }
            }
          }
          
          handleSettingsUpdate(message);
        } else {
          Serial.println("Empty WebSocket message received");
        }
      }
      break;
  }
}

void AfterburnerWiFiService::handleSettingsUpdate(const String& jsonData) {
  // Rate limiting: only process settings updates every 200ms (increased for stability)
  if (millis() - lastSettingsUpdate < 200) {
    Serial.println("Settings update rate limited - skipping");
    return;
  }
  lastSettingsUpdate = millis();
  
  Serial.print("Received JSON: ");
  Serial.println(jsonData);
  Serial.printf("JSON length: %d\n", jsonData.length());
  
  // Check available memory before processing
  Serial.printf("Free heap before processing: %d bytes\n", ESP.getFreeHeap());
  
  // Add watchdog feed to prevent resets
  ESP.wdtFeed();
  
  // Use smaller JSON document to prevent memory issues
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, jsonData);
  
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    Serial.printf("Error code: %d\n", error.code());
    ESP.wdtFeed(); // Feed watchdog before returning
    return;
  }
  
  Serial.println("JSON parsed successfully");
  Serial.printf("JSON document size: %d bytes\n", doc.memoryUsage());
  
  // Get current settings and create a working copy
  AfterburnerSettings& currentSettings = settingsManager->getSettings();
  AfterburnerSettings newSettings = currentSettings; // Copy current settings
  bool settingsChanged = false;
  
  Serial.println("=== Processing Settings ===");
  Serial.printf("Current settings - Mode: %d, Speed: %d, Brightness: %d, LEDs: %d, Threshold: %d\n",
                currentSettings.mode, currentSettings.speedMs, currentSettings.brightness, currentSettings.numLeds, currentSettings.abThreshold);
  Serial.printf("Current colors - Start: [%d,%d,%d], End: [%d,%d,%d]\n",
                currentSettings.startColor[0], currentSettings.startColor[1], currentSettings.startColor[2],
                currentSettings.endColor[0], currentSettings.endColor[1], currentSettings.endColor[2]);
  
  // Handle mode update
  if (doc.containsKey("mode")) {
    Serial.println("Found 'mode' in JSON");
    int newMode = doc["mode"] | 0; // Force to int
    Serial.printf("Mode value from JSON: %d\n", newMode);
    if (newMode >= 0 && newMode <= 2) {  // Valid mode range
      if (newMode != newSettings.mode) {
        newSettings.mode = (uint8_t)newMode;
        settingsChanged = true;
        Serial.printf("Mode updated to: %d\n", newMode);
      } else {
        Serial.println("Mode unchanged");
      }
    } else {
      Serial.printf("Invalid mode value: %d (must be 0-2)\n", newMode);
    }
  } else {
    Serial.println("No 'mode' found in JSON");
  }
  
  // Handle start color update
  if (doc.containsKey("startColor")) {
    Serial.println("Found 'startColor' in JSON");
    JsonArray startColorArray = doc["startColor"];
    Serial.printf("Start color array size: %d\n", startColorArray.size());
    if (startColorArray.size() == 3) {
      // Get color values and force to int
      int newR = startColorArray[0] | 0;
      int newG = startColorArray[1] | 0;
      int newB = startColorArray[2] | 0;
      Serial.printf("Start color values from JSON: R=%d, G=%d, B=%d\n", newR, newG, newB);
        
                 // Validate color values are in valid range
         if (newR >= 0 && newR <= 255 && newG >= 0 && newG <= 255 && newB >= 0 && newB <= 255) {
           if (newR != currentSettings.startColor[0] || newG != currentSettings.startColor[1] || newB != currentSettings.startColor[2]) {
             newSettings.startColor[0] = (uint8_t)newR;
             newSettings.startColor[1] = (uint8_t)newG;
             newSettings.startColor[2] = (uint8_t)newB;
             settingsChanged = true;
             Serial.printf("Start color updated to: R=%d, G=%d, B=%d\n", newR, newG, newB);
           }
         } else {
           Serial.printf("Invalid start color values: R=%d, G=%d, B=%d (must be 0-255)\n", newR, newG, newB);
         }
             // Color values are automatically converted to int with | 0
    } else {
      Serial.printf("Invalid start color array size: %d (expected 3)\n", startColorArray.size());
    }
  }
  
  // Handle end color update
  if (doc.containsKey("endColor")) {
    JsonArray endColorArray = doc["endColor"];
    if (endColorArray.size() == 3) {
      // Get color values and force to int
      int newR = endColorArray[0] | 0;
      int newG = endColorArray[1] | 0;
      int newB = endColorArray[2] | 0;
        
                 // Validate color values are in valid range
         if (newR >= 0 && newR <= 255 && newG >= 0 && newG <= 255 && newB >= 0 && newB <= 255) {
           if (newR != currentSettings.endColor[0] || newG != currentSettings.endColor[1] || newB != currentSettings.endColor[2]) {
             newSettings.endColor[0] = (uint8_t)newR;
             newSettings.endColor[1] = (uint8_t)newG;
             newSettings.endColor[2] = (uint8_t)newB;
             settingsChanged = true;
             Serial.printf("End color updated to: R=%d, G=%d, B=%d\n", newR, newG, newB);
           }
         } else {
           Serial.printf("Invalid end color values: R=%d, G=%d, B=%d (must be 0-255)\n", newR, newG, newB);
         }
             // Color values are automatically converted to int with | 0
    } else {
      Serial.printf("Invalid end color array size: %d (expected 3)\n", endColorArray.size());
    }
  }
  
  // Handle speed update
  if (doc.containsKey("speedMs")) {
    int newSpeed = doc["speedMs"] | 0; // Force to int
    if (newSpeed >= 100 && newSpeed <= 5000) {  // Valid speed range
      if (newSpeed != currentSettings.speedMs) {
        newSettings.speedMs = (uint16_t)newSpeed;
        settingsChanged = true;
        Serial.printf("Speed updated to: %d ms\n", newSpeed);
      }
    } else {
      Serial.printf("Invalid speed value: %d (must be 100-5000)\n", newSpeed);
    }
  }
  
  // Handle brightness update
  if (doc.containsKey("brightness")) {
    int newBrightness = doc["brightness"] | 0; // Force to int
    if (newBrightness >= 10 && newBrightness <= 255) {  // Valid brightness range
      if (newBrightness != currentSettings.brightness) {
        newSettings.brightness = (uint8_t)newBrightness;
        settingsChanged = true;
        Serial.printf("Brightness updated to: %d\n", newBrightness);
      }
    } else {
      Serial.printf("Invalid brightness value: %d (must be 10-255)\n", newBrightness);
    }
  }
  
  // Handle numLeds update
  if (doc.containsKey("numLeds")) {
    int newNumLeds = doc["numLeds"] | 0; // Force to int
    if (newNumLeds >= 1 && newNumLeds <= 300) {  // Valid LED count range
      if (newNumLeds != currentSettings.numLeds) {
        newSettings.numLeds = (uint16_t)newNumLeds;
        settingsChanged = true;
        Serial.printf("Number of LEDs updated to: %d\n", newNumLeds);
      }
    } else {
      Serial.printf("Invalid numLeds value: %d (must be 1-300)\n", newNumLeds);
    }
  }
  
  // Handle abThreshold update
  if (doc.containsKey("abThreshold")) {
    int newThreshold = doc["abThreshold"] | 0; // Force to int
    if (newThreshold >= 0 && newThreshold <= 100) {  // Valid threshold range
      if (newThreshold != currentSettings.abThreshold) {
        newSettings.abThreshold = (uint8_t)newThreshold;
        settingsChanged = true;
        Serial.printf("Afterburner threshold updated to: %d%%\n", newThreshold);
      }
    } else {
      Serial.printf("Invalid abThreshold value: %d (must be 0-100)\n", newThreshold);
    }
  }
  
  // Only save if settings actually changed
  if (settingsChanged) {
    // Copy the new settings back to the original settings object
    currentSettings = newSettings;
    
    Serial.println("=== SETTINGS CHANGED ===");
    Serial.printf("Final settings - Mode: %d, Speed: %d, Brightness: %d, LEDs: %d, Threshold: %d\n",
                  currentSettings.mode, currentSettings.speedMs, currentSettings.brightness, currentSettings.numLeds, currentSettings.abThreshold);
    Serial.printf("Final colors - Start: [%d,%d,%d], End: [%d,%d,%d]\n",
                  currentSettings.startColor[0], currentSettings.startColor[1], currentSettings.startColor[2],
                  currentSettings.endColor[0], currentSettings.endColor[1], currentSettings.endColor[2]);
    
    Serial.println("Settings changed, will save to EEPROM in main loop");
    settingsNeedSave = true;
  } else {
    Serial.println("No settings changed");
  }
  
  // Final watchdog feed
  ESP.wdtFeed();
}

void AfterburnerWiFiService::updateStatus(float throttle, uint8_t mode) {
  lastThrottle = throttle;
  // Send status notification every 500ms (reduced frequency to prevent memory pressure)
  if (millis() - lastStatusUpdate > 500) {
    ESP.wdtFeed(); // Feed watchdog before status update
    sendStatusToClient();
    lastStatusUpdate = millis();
    ESP.wdtFeed(); // Feed watchdog after status update
  }
}

void AfterburnerWiFiService::sendStatusToClient() {
  if (!clientConnected || !webSocket) return;
  
  // Get current settings for mode
  AfterburnerSettings& settings = settingsManager->getSettings();
  
  StaticJsonDocument<256> doc;
  doc["type"] = "status";
  doc["thr"] = round(lastThrottle * 100) / 100.0;
  doc["mode"] = settings.mode;
  
  // Add throttle calibration data
  doc["signalValid"] = throttleReader->isSignalValid();
  doc["pulseCount"] = throttleReader->getPulseCount();
  doc["invalidPulseCount"] = throttleReader->getInvalidPulseCount();
  doc["calibrating"] = throttleReader->isCalibrating();
  doc["calibrationComplete"] = throttleReader->isCalibrationComplete();
  
  if (throttleReader->isCalibrationComplete()) {
    doc["minPulse"] = throttleReader->getMinPulse();
    doc["maxPulse"] = throttleReader->getMaxPulse();
    doc["pulseRange"] = throttleReader->getMaxPulse() - throttleReader->getMinPulse();
  } else {
    // Use default values when calibration is not complete
    doc["minPulse"] = PWM_MIN_PULSE;
    doc["maxPulse"] = PWM_MAX_PULSE;
    doc["pulseRange"] = PWM_MAX_PULSE - PWM_MIN_PULSE;
  }
  
  String jsonString;
  serializeJson(doc, jsonString);
  webSocket->broadcastTXT(jsonString);
}

bool AfterburnerWiFiService::isConnected() {
  return clientConnected;
}

void AfterburnerWiFiService::handleRoot() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Afterburner Control</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .container { max-width: 600px; margin: 0 auto; }
        .section { margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }
        .status { background: #f0f0f0; padding: 10px; border-radius: 3px; }
        input, select { width: 100%; padding: 8px; margin: 5px 0; box-sizing: border-box; }
        button { background: #007bff; color: white; padding: 10px 20px; border: none; border-radius: 3px; cursor: pointer; }
        button:hover { background: #0056b3; }
        .color-input { display: flex; gap: 10px; }
        .color-input input { flex: 1; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Afterburner Control</h1>
        
        <div class="section">
            <h3>Status</h3>
            <div class="status" id="status">Connecting...</div>
        </div>
        
        <div class="section">
            <h3>Settings</h3>
            <form id="settingsForm">
                <label>Mode:</label>
                <select id="mode">
                    <option value="0">Static</option>
                    <option value="1">Pulse</option>
                    <option value="2">Afterburner</option>
                </select>
                
                <label>Start Color (RGB):</label>
                <div class="color-input">
                    <input type="number" id="startR" min="0" max="255" placeholder="R">
                    <input type="number" id="startG" min="0" max="255" placeholder="G">
                    <input type="number" id="startB" min="0" max="255" placeholder="B">
                </div>
                
                <label>End Color (RGB):</label>
                <div class="color-input">
                    <input type="number" id="endR" min="0" max="255" placeholder="R">
                    <input type="number" id="endG" min="0" max="255" placeholder="G">
                    <input type="number" id="endB" min="0" max="255" placeholder="B">
                </div>
                
                <label>Speed (ms):</label>
                <input type="number" id="speedMs" min="100" max="5000" step="100">
                
                <label>Brightness:</label>
                <input type="range" id="brightness" min="10" max="255" step="5">
                <span id="brightnessValue">128</span>
                
                <label>Number of LEDs:</label>
                <input type="number" id="numLeds" min="1" max="300">
                
                <label>Afterburner Threshold (%):</label>
                <input type="number" id="abThreshold" min="0" max="100">
                
                <button type="submit">Update Settings</button>
            </form>
        </div>
    </div>
    
    <script>
        let ws = new WebSocket('ws://' + window.location.hostname + ':81/');
        
        ws.onopen = function() {
            document.getElementById('status').innerHTML = 'Connected';
        };
        
        ws.onclose = function() {
            document.getElementById('status').innerHTML = 'Disconnected';
        };
        
        ws.onmessage = function(event) {
            const data = JSON.parse(event.data);
            
            if (data.type === 'settings') {
                document.getElementById('mode').value = data.mode;
                document.getElementById('startR').value = data.startColor[0];
                document.getElementById('startG').value = data.startColor[1];
                document.getElementById('startB').value = data.startColor[2];
                document.getElementById('endR').value = data.endColor[0];
                document.getElementById('endG').value = data.endColor[1];
                document.getElementById('endB').value = data.endColor[2];
                document.getElementById('speedMs').value = data.speedMs;
                document.getElementById('brightness').value = data.brightness;
                document.getElementById('brightnessValue').textContent = data.brightness;
                document.getElementById('numLeds').value = data.numLeds;
                document.getElementById('abThreshold').value = data.abThreshold;
            } else if (data.type === 'status') {
                document.getElementById('status').innerHTML = 
                    'Throttle: ' + data.thr + ' | Mode: ' + data.mode;
            }
        };
        
        document.getElementById('brightness').addEventListener('input', function() {
            document.getElementById('brightnessValue').textContent = this.value;
        });
        
        document.getElementById('settingsForm').addEventListener('submit', function(e) {
            e.preventDefault();
            
            const settings = {
                mode: parseInt(document.getElementById('mode').value),
                startColor: [
                    parseInt(document.getElementById('startR').value),
                    parseInt(document.getElementById('startG').value),
                    parseInt(document.getElementById('startB').value)
                ],
                endColor: [
                    parseInt(document.getElementById('endR').value),
                    parseInt(document.getElementById('endG').value),
                    parseInt(document.getElementById('endB').value)
                ],
                speedMs: parseInt(document.getElementById('speedMs').value),
                brightness: parseInt(document.getElementById('brightness').value),
                numLeds: parseInt(document.getElementById('numLeds').value),
                abThreshold: parseInt(document.getElementById('abThreshold').value)
            };
            
            ws.send(JSON.stringify(settings));
        });
    </script>
</body>
</html>
  )";
  
  webServer->send(200, "text/html", html);
}

void AfterburnerWiFiService::handleSettings() {
  AfterburnerSettings& settings = settingsManager->getSettings();
  
  StaticJsonDocument<256> doc;
  doc["mode"] = settings.mode;
  JsonArray startColorArray = doc.createNestedArray("startColor");
  startColorArray.add(settings.startColor[0]);
  startColorArray.add(settings.startColor[1]);
  startColorArray.add(settings.startColor[2]);
  
  JsonArray endColorArray = doc.createNestedArray("endColor");
  endColorArray.add(settings.endColor[0]);
  endColorArray.add(settings.endColor[1]);
  endColorArray.add(settings.endColor[2]);
  doc["speedMs"] = settings.speedMs;
  doc["brightness"] = settings.brightness;
  doc["numLeds"] = settings.numLeds;
  doc["abThreshold"] = settings.abThreshold;
  
  String jsonString;
  serializeJson(doc, jsonString);
  webServer->send(200, "application/json", jsonString);
}

void AfterburnerWiFiService::handleUpdateSettings() {
  if (webServer->hasArg("plain")) {
    String jsonData = webServer->arg("plain");
    handleSettingsUpdate(jsonData);
    webServer->send(200, "text/plain", "Settings updated");
  } else {
    webServer->send(400, "text/plain", "No data received");
  }
}

void AfterburnerWiFiService::handleStatus() {
  StaticJsonDocument<128> doc;
  doc["connected"] = clientConnected;
  doc["clients"] = webSocket ? webSocket->connectedClients() : 0;
  
  String jsonString;
  serializeJson(doc, jsonString);
  webServer->send(200, "application/json", jsonString);
}
