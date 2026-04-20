// Potentiometer_Logic.h
#include <Arduino.h>

int readPot(int pin) {
  int raw = analogRead(pin);

  if (raw > 0) {
    return raw;
  }
  else return 0;
}