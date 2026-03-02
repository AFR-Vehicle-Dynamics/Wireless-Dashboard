#pragma once
#include <Arduino.h>

namespace Pin {
    // IMU (SPI is also using 23, 18, 19)
    static constexpr uint8_t  BNO08X_CS = 32;
    static constexpr uint8_t  BNO08X_INT = 33;
    static constexpr uint8_t  BNO08X_RESET = 25;
    
    //Thermals
    static constexpr uint8_t AIR_TEMP = 1; // Radiator Air Intake
    static constexpr uint8_t COOLANT_IN = 2; // Engine Coolant Inlet
    static constexpr uint8_t COOLANT_OUT = 3; // Engine Coolant Outlet

    //Pots
    static constexpr uint8_t LINEAR1 = 36;
    static constexpr uint8_t STEERING = 34;
}