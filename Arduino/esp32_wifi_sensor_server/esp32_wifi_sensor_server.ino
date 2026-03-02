// esp32_wifi_sensor_server.ino
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "src/config/pins.h"
#include "src/modules/Dashboard_UI.h"
#include "src/modules/Thermal_Logic.h"
#include "src/modules/Potentiometer_Logic.h"
#include "src/modules/IMU.h"

IMU imu;

// Network Configuration
const char* ssid = "Banana phone";
const char* password = "1234abcd";

AsyncWebServer server(8080);
AsyncWebSocket ws("/ws");

// System Timing
const int SAMPLE_RATE = 100; // milliseconds between readings
unsigned long lastReadTime = 0;
int sampleCount = 0;

// WebSocket Event Handler
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
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

void setup() {
  Serial.begin(115200);

  // ADC Configuration
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  imu.setup();
  
  // Wifi Initalization
  Serial.println("\nConnecting to Wifi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWifi Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Web Socket Configuration
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // Web Server Routing
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", index_html); // Serves UI from Dashboard_UI.h
  });

  server.begin();
  Serial.println("Upload Worked Correctly.");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastReadTime >= SAMPLE_RATE) {
    lastReadTime = currentTime;

    int linearPot1 = readPot(Pin::LINEAR1);
    int currentSteer = readPot(Pin::STEERING);

    imu.update();
    float pitch = imu.getData().pitch;
    float yaw = imu.getData().yaw;
    float roll = imu.getData().roll;
    
    // Constructing the JSON packet
    String jsonData = "{";
    jsonData += "\"raw\":" + String(linearPot1) + ",";
    jsonData += "\"steer\":" + String(currentSteer) + ",";
    jsonData += "\"air\":" + String(getCelsius(Pin::AIR_TEMP), 1) + ",";
    jsonData += "\"c1\":" + String(getCelsius(Pin::COOLANT_IN), 1) + ",";
    jsonData += "\"c2\":" + String(getCelsius(Pin::COOLANT_OUT), 1) + ",";
    jsonData += "\"pitch\":" + String(pitch) + ",";
    jsonData += "\"roll\":" + String(roll) + ",";
    jsonData += "\"yaw\":" + String(yaw) + ",";
    jsonData += "\"sample\":" + String(sampleCount++);
    jsonData += "}";
    
    // Send to all connected WebSocket clients
    ws.textAll(jsonData);

    // Print to Serial for debugging
    Serial.println(jsonData);
  }

  // Clean up disconnected clients
  ws.cleanupClients();
}