#pragma once
#include <Adafruit_BNO08x.h>

struct euler_t {
    float yaw;
    float pitch;
    float roll;
};

class IMU
{
private:
    sh2_SensorValue_t sensorValue;
    euler_t orientation;
    Adafruit_BNO08x bno08x;
public:
    IMU();
    void setReports(sh2_SensorId_t reportType, long report_interval);
    void setup(void);
    void update();
    euler_t getData() const;
};