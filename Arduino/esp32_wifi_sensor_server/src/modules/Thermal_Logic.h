// Thermal_Logic.h
#include <Arduino.h>

// Calibration and Sensor Parameters
const float R_BALANCE = 10000.0;
const float CAL_OFFSET = 2.7;

// Steinhart-Hart Coefficients for thermistors
const float STEIN_A = 1.129148e-03;
const float STEIN_B = 2.34125e-04;
const float STEIN_C = 8.76741e-08;

// Converts raw ADC data to Celsius using the Steinhart-Hart equation
float getCelsius(int pin) {
  int rawAdc = analogRead(pin);

  // Returns -31.5 to signal a hardware error or open circuit
  //if (rawAdc <= 100 || rawAdc >= 4000) return -31.5;

  // Calculate thermistor resistance based on the 3.3V reference voltage
  float voltage = (rawAdc * 3.3) / 4095.0;
  float r_therm = R_BALANCE * (voltage / (3.3 - voltage));

  // Steinhart-Hart Equation implementation
  float logR = log(r_therm);
  float tempK = 1.0 / (STEIN_A + (STEIN_B * logR) + (STEIN_C * pow(logR, 3)));

  // Convert Kelvin to Celsius and apply final calibration offset
  return (tempK - 273.15) - CAL_OFFSET;
}