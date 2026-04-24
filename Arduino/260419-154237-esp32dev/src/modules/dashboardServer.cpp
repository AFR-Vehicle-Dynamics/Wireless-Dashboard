#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#include "dashboardServer.h"
#include "Dashboard_UI.h"

// WebSocket Event Handler
void dashboardServer::onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      // Log when a new team member opens the dashboard
      Serial.printf("WebSocket client #%u connected\n", client->id());
    } else if (type == WS_EVT_DISCONNECT) {
      // Log when a client closes the browser
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
    } else if (type == WS_EVT_DATA) {
      // Log any incoming commands from the dashboard
      Serial.printf("Data received from client #%u: %s\n", client->id(), (const char*)data);
    }
  }

void dashboardServer::setup() {
  
  // Wifi Initalization
  Serial.println("\nConnecting to Wifi...");
  WiFi.begin(dashboardServer::ssid, dashboardServer::password);

int time = 0; 
while (WiFi.status() != WL_CONNECTED && time < 10000) { 
  delay(500);
  Serial.print(".");
  time += 500;
} 

if (WiFi.status() != WL_CONNECTED) { 
  Serial.println("\nFailed to connect to Wifi.");
  return;
} else {
  Serial.println("\nWifi Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

  // Web Socket Configuration
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // Web Server Routing
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", index_html); // Serves UI from Dashboard_UI.h
  });

  server.begin();
  Serial.println("Upload Worked Correctly.");
};

void dashboardServer::sendData(String data){ws.textAll(data);};
void dashboardServer::cleanClients(){ws.cleanupClients();};