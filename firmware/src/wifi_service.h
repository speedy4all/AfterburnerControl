#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "settings.h"
#include "throttle.h"

// WiFi configuration
#define WIFI_SSID "Afterburner_AP"
#define WIFI_PASSWORD "afterburner123"
#define WEB_SERVER_PORT 80
#define WEB_SOCKET_PORT 81

class AfterburnerWiFiService {
private:
  ESP8266WebServer* webServer;
  WebSocketsServer* webSocket;
  SettingsManager* settingsManager;
  ThrottleReader* throttleReader;
  
  // Status notification timer
  unsigned long lastStatusUpdate;
  
  // Connection status
  bool clientConnected;
  
  // Last throttle value for status updates
  float lastThrottle;
  
  // Rate limiting for settings updates
  unsigned long lastSettingsUpdate;
  
  // Deferred save flag
  bool settingsNeedSave;
  
public:
  AfterburnerWiFiService(SettingsManager* settings, ThrottleReader* throttle);
  void begin();
  void updateStatus(float throttle, uint8_t mode);
  bool isConnected();
  void loop();
  
  // WebSocket event handlers
  void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
  
  // Web server handlers
  void handleRoot();
  void handleSettings();
  void handleUpdateSettings();
  void handleStatus();
  
private:
  void setupWebServer();
  void setupWebSocket();
  void sendStatusToClient();
  void handleSettingsUpdate(const String& jsonData);
};

#endif // WIFI_SERVICE_H
