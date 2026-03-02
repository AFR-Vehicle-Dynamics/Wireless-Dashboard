// Potentiometer_Logic.h
#include <Arduino.h>

int readPot(int pin) {
  int raw = analogRead(pin);
  return raw;
}