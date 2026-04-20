// esp32_wifi_sensor_server.ino
#include <WiFi.h>
#include "config/pins.h"
#include "modules/Dashboard_UI.h"
#include "modules/Thermal_Logic.cpp"
#include "modules/Potentiometer_Logic.cpp"
#include "modules/IMU.h"
#include "modules/dashboardServer.h"

IMU imu;
dashboardServer server;

// System Timing
const int SAMPLE_RATE = 100; // milliseconds between readings
unsigned long lastReadTime = 0;
int sampleCount = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);

  // ADC Configuration
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  imu.setup();
  server.setup();
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastReadTime >= SAMPLE_RATE) {
    lastReadTime = currentTime;

    int linearPot1 = readPot(Pin::LINEAR1);
    int linearPot2 = readPot(Pin::LINEAR2);

    int currentSteer = readPot(Pin::STEERING);

    imu.update();
    float pitch = imu.getData().pitch;
    float yaw = imu.getData().yaw;
    float roll = imu.getData().roll;
    float xaccel = imu.getData().xaccel;
    float yaccel = imu.getData().yaccel;
    float zaccel = imu.getData().zaccel;
    
    // Constructing the JSON packet
    String jsonData = "{";
    jsonData += "\"raw\":" + String(linearPot1) + ",";
    jsonData += "\"raw2\":" + String(linearPot1) + ",";
    jsonData += "\"steer\":" + String(currentSteer) + ",";
    jsonData += "\"air\":" + String(getCelsius(Pin::AIR_TEMP), 1) + ",";
    jsonData += "\"c1\":" + String(getCelsius(Pin::COOLANT_IN), 1) + ",";
    jsonData += "\"c2\":" + String(getCelsius(Pin::COOLANT_OUT), 1) + ",";
    jsonData += "\"pitch\":" + String(pitch) + ",";
    jsonData += "\"roll\":" + String(roll) + ",";
    jsonData += "\"yaw\":" + String(yaw) + ",";
    jsonData += "\"xaccel\":" + String(xaccel) + ",";
    jsonData += "\"yaccel\":" + String(yaccel) + ",";
    jsonData += "\"zaccel\":" + String(zaccel) + ",";
    jsonData += "\"sample\":" + String(sampleCount++);
    jsonData += "}";
    
    // Send to all connected WebSocket clients
    server.sendData(jsonData);

    // Print to Serial for debugging
    Serial.println(jsonData);
  }

  // Clean up disconnected clients
  server.cleanClients();
}