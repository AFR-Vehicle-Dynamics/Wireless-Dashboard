#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

// wifi host login
const char* ssid = "Goober Fiber";
const char* password = "WifiPassword123";

// Pins
const int ANALOG_PIN = 34;

AsyncWebServer server(8080);
AsyncWebSocketMessageHandler wsHandler;
AsyncWebSocket ws("/ws", wsHandler.eventHandler());

// Sampling
const int SAMPLE_RATE = 100;  // milliseconds between readings
unsigned long lastReadTime = 0;

// HTML/JavaScript for web interface
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
<h1>Linear pot data</h1>
<div id="data">Waiting...</div>
<canvas id="LP_chart" width="800" height="300"></canvas>

<h1>Steering angle</h1>
<div class="steering">
    <p id="Steering_angle">No data</p>
    <canvas id="Steering_chart" width="600" height="300"></canvas>
</div>

<script>
// Charts setup
const LPctx = document.getElementById('LP_chart').getContext('2d');
const maxDataPoints = 50;
const linearPotData = {
  labels: [],
  datasets: [{
    label: 'Shock 1',
    data: [],
    borderColor: '#4CAF50',
    borderWidth: 2,
    fill: false
  }]
};
const Steeringctx = document.getElementById('Steering_chart').getContext('2d');
const steeringData = {
  labels: [],
  datasets: [{
    label: 'angle',
    data: [4096, 0],
    backgroundColor: [
      'rgb(255, 99, 132)',
      'rgb(54, 162, 235)'
    ],
    hoverOffset: 0,
    clip: -350,
  }]
};

const chart = new Chart(LPctx, {
  type: 'line',
  data: linearPotData,
  options: {
    responsive: false,
    animation: { duration: 0 },
    scales: {
      y: {
        beginAtZero: true,
        max: 4096
      }
    }
  }
});

const Steeringchart = new Chart(Steeringctx, {
  type: 'doughnut',
  data: steeringData,
  options: {
    cutout: "80%",
    circumference: 270,
    rotation: -135,
    radius: "100",
  }
});
// WebSocket
var ws = new WebSocket('ws://192.168.1.112:8080/ws');
//test server
//var ws = new WebSocket('ws://127.0.0.1:8080');
var sampleCount = 0;

ws.onopen = function() { 
  document.getElementById('data').innerHTML = 'Connected'; 
};

ws.onmessage = function(e) {
    const data = JSON.parse(e.data);
    if (data.message === 'connected') return;
    //console.log(e.data);
      updateCounter = 0;

      // Update DOM
      document.getElementById('data').innerHTML = 'Value: ' + data.raw;
      document.getElementById('Steering_angle').innerHTML = (-135 + 270 * data.angle/4096).toFixed(2) + '°';

      // Update charts
      linearPotData.labels.push(sampleCount++);
      linearPotData.datasets[0].data.push(data.raw);

      if (linearPotData.labels.length > maxDataPoints) {
          linearPotData.labels.shift();
          linearPotData.datasets[0].data.shift();
      }

      angle = data.angle;
      steeringData.datasets[0].data[0] = angle;
      steeringData.datasets[0].data[1] = 4096 - angle;


      chart.update();
      Steeringchart.update();
    
};

ws.onerror = function(e) { 
  document.getElementById('data').innerHTML = 'ERROR'; 
};
</script>
<style>
    .steering {
        position:relative;
        height: 300px; 
        width: 300px;
    }
    #Steering_angle {
        position: absolute;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
    }
</style>
</body>
</html>
)rawliteral";

// WebSocket event handler
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
                       AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client #%u connected\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
  }
}

void setup() {
  Serial.begin(115200);
  
  // Configure ADC
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  
  // Connect to WiFi
  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Setup WebSocket
  wsHandler.onConnect([](AsyncWebSocket *server, AsyncWebSocketClient *client) {
    Serial.printf("WebSocket client #%u connected\n", client->id());
  });

  wsHandler.onDisconnect([](AsyncWebSocket *server, uint32_t clientId) {
    Serial.printf("WebSocket client #%u disconnected\n", clientId);
  });

  wsHandler.onMessage([](AsyncWebSocket *server, AsyncWebSocketClient *client, const uint8_t *data, size_t len) {
    Serial.printf("Received from client #%u: %s\n", client->id(), (const char *)data);
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  server.addHandler(&ws);

  server.begin();
  Serial.println("upload worked correctly");
}

int sampleCount = 0;

void loop() {
  unsigned long currentTime = millis();
  
  // Read and send sensor data at specified rate
  if (currentTime - lastReadTime >= SAMPLE_RATE) {
    lastReadTime = currentTime;
    
    // Read sensor
    int rawValue = analogRead(ANALOG_PIN);
    
    // Create JSON data
    String jsonData = "{";
    jsonData += "\"raw\":" + String(rawValue) + ",";
    jsonData += "\"angle\":" + String(rawValue) + ",";
    jsonData += "\"sample\":" + String(sampleCount++);
    jsonData += "}";
    
    // Send to all connected WebSocket clients
    ws.textAll(jsonData);
    
    // Also print to Serial for debugging
    Serial.println(jsonData);
  }
  
  // Clean up disconnected clients
  ws.cleanupClients();
}
