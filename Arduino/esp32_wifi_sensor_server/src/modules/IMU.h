#pragma once
#include <Adafruit_BNO08x.h>

struct euler_t {
    float pitch;
    float roll;
    float yaw;
    float xaccel;
    float yaccel;
    float zaccel;
};

class IMU
{
private:
    sh2_SensorValue_t sensorValue;
    euler_t orientation;
    Adafruit_BNO08x bno08x;
public:
    IMU();
    void setup(void);
    void setReports(void);
    void update();
    euler_t getData() const;
};