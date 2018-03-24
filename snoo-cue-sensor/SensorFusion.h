// This file is a part of Snoo project.
// Copyright 2018 Aleksander Gajewski <adiog@brainfuck.pl>.


#ifndef SNOO_CUE_SENSOR_SENSORFUSION_H
#define SNOO_CUE_SENSOR_SENSORFUSION_H

#include <Kalman.h>

#define RESTRICT_PITCH  // Comment out to restrict roll to ±90deg instead - please read: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf


class SensorFusion
{
public:
    explicit SensorFusion(float complementaryWeight = 0.93F) noexcept;
    void init(float *accelerometerRawValues);

    void apply(
        const float accelerometerRawValues[3],
        const float gyroscopeRawValues[3],
        float magnetometerRawValues[3],
        float xAngleAfterFusion[3],
        float yAngleAfterFusion[3],
        float timeDelta);

private:
    Kalman kalmanX;
    Kalman kalmanY;

    float accX, accY, accZ;
    float gyroX, gyroY, gyroZ;

    float gyroXangle, gyroYangle;  // Angle calculated using the gyro only
    float compAngleX, compAngleY;  // Angle calculated using a complementary filter
    float kalAngleX, kalAngleY;    // Angle calculated using a Kalman filter

    float complementaryWeight;
};


#endif
