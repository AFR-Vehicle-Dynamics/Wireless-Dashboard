// Potentiometer_Logic.h
#include <Arduino.h>

// Pin Mapping
const int PIN_POT_INPUT = 34;

// Placeholder for steering angle calculations
float calculateSteeringAngle(int rawValue) {
  // Logic to be implemented
  if (rawValue == -1) return -1.0; // No Data passthrough

  return 0.0; // Placeholder for final mapping
}

// Wrapper function to acquire raw sensor data from the ADC
int readPotRaw() {
  int raw = analogRead(PIN_POT_INPUT);

  // Returns -1 if no physical sensor is detected
  if (raw < 50) return -1;

  return raw;
}