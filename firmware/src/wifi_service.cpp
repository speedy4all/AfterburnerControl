#include "wifi_service.h"

// Static instance for WebSocket callback
static AfterburnerWiFiService* wifiServiceInstance = nullptr;

// WebSocket event handler wrapper
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (wifiServiceInstance) {
    wifiServiceInstance->onWebSocketEvent(num, type, payload, length);
  }
}

AfterburnerWiFiService::AfterburnerWiFiService(SettingsManager* settings) {
  settingsManager = settings;
  webServer = nullptr;
  webSocket = nullptr;
  lastStatusUpdate = 0;
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
}

void AfterburnerWiFiService::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
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
          
          handleSettingsUpdate(message);
        } else {
          Serial.println("Empty WebSocket message received");
        }
      }
      break;
  }
}

void AfterburnerWiFiService::handleSettingsUpdate(const String& jsonData) {
  Serial.print("Received JSON: ");
  Serial.println(jsonData);
  
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, jsonData);
  
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    return;
  }
  
  AfterburnerSettings& settings = settingsManager->getSettings();
  bool settingsChanged = false;
  
  if (doc.containsKey("mode")) {
    settings.mode = doc["mode"];
    settingsChanged = true;
  }
  
  if (doc.containsKey("startColor")) {
    JsonArray startColorArray = doc["startColor"];
    if (startColorArray.size() == 3) {
      settings.startColor[0] = startColorArray[0];
      settings.startColor[1] = startColorArray[1];
      settings.startColor[2] = startColorArray[2];
      settingsChanged = true;
    }
  }
  
  if (doc.containsKey("endColor")) {
    JsonArray endColorArray = doc["endColor"];
    if (endColorArray.size() == 3) {
      settings.endColor[0] = endColorArray[0];
      settings.endColor[1] = endColorArray[1];
      settings.endColor[2] = endColorArray[2];
      settingsChanged = true;
    }
  }
  
  if (doc.containsKey("speedMs")) {
    settings.speedMs = doc["speedMs"];
    settingsChanged = true;
  }
  
  if (doc.containsKey("brightness")) {
    settings.brightness = doc["brightness"];
    settingsChanged = true;
  }
  
  if (doc.containsKey("numLeds")) {
    settings.numLeds = doc["numLeds"];
    settingsChanged = true;
  }
  
  if (doc.containsKey("abThreshold")) {
    settings.abThreshold = doc["abThreshold"];
    settingsChanged = true;
  }
  
  if (settingsChanged) {
    settingsManager->saveSettings();
    Serial.println("Settings updated via WebSocket");
  } else {
    Serial.println("No settings changed");
  }
}

void AfterburnerWiFiService::updateStatus(float throttle, uint8_t mode) {
  lastThrottle = throttle;
  // Send status notification every 200ms
  if (millis() - lastStatusUpdate > 200) {
    sendStatusToClient();
    lastStatusUpdate = millis();
  }
}

void AfterburnerWiFiService::sendStatusToClient() {
  if (!clientConnected || !webSocket) return;
  
  // Get current settings for mode
  AfterburnerSettings& settings = settingsManager->getSettings();
  
  StaticJsonDocument<128> doc;
  doc["type"] = "status";
  doc["thr"] = round(lastThrottle * 100) / 100.0;
  doc["mode"] = settings.mode;
  
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
