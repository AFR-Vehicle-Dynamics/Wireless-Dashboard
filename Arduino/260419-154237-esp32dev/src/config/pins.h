#pragma once
#include <Arduino.h>

namespace Pin {
    // IMU (SPI is also using 23, 18, 19)
    static constexpr uint8_t  BNO08X_CS = 5;
    static constexpr uint8_t  BNO08X_INT = 25;
    static constexpr uint8_t  BNO08X_RESET = 26;
    
    //Thermals
    static constexpr uint8_t AIR_TEMP = 35; // Radiator Air Intake
    static constexpr uint8_t COOLANT_IN = 32; // Engine Coolant Inlet
    static constexpr uint8_t COOLANT_OUT = 33; // Engine Coolant Outlet

    //Pots
    static constexpr uint8_t LINEAR1 = 34;
    static constexpr uint8_t LINEAR2 = 36;
    static constexpr uint8_t STEERING = 39;

    //GPS
    static constexpr uint8_t GPS_TX = 21;
    static constexpr uint8_t GPS_RX = 22;
}
