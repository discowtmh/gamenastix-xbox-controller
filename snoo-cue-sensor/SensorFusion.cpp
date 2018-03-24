// This file is a part of Snoo project.
// Copyright 2018 Aleksander Gajewski <adiog@brainfuck.pl>.

#include "SensorFusion.h"
#include <Arduino.h>


const float RAD_TO_DEG_FLOAT = (float)RAD_TO_DEG;

SensorFusion::SensorFusion(float complementaryWeight) noexcept
        : complementaryWeight(complementaryWeight)
{
}

void SensorFusion::init(float *accelerometerRawValues)
{
    accX = accelerometerRawValues[0];
    accY = accelerometerRawValues[1];
    accZ = accelerometerRawValues[2];

// Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
// atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
// It is then converted from radians to degrees
#ifdef RESTRICT_PITCH  // Eq. 25 and 26
    float roll = atan2f(accY, accZ) * RAD_TO_DEG_FLOAT;
    float pitch = atanf(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG_FLOAT;
#else  // Eq. 28 and 29
    float roll = atanf(accY / sqrtf(accX * accX + accZ * accZ)) * RAD_TO_DEG_FLOAT;
    float pitch = atan2f(-accX, accZ) * RAD_TO_DEG_FLOAT;
#endif

    kalmanX.setAngle(roll);
    kalmanY.setAngle(pitch);
    gyroXangle = roll;
    gyroYangle = pitch;
    compAngleX = roll;
    compAngleY = pitch;
}

void SensorFusion::apply(
    const float accelerometerRawValues[3],
    const float gyroscopeRawValues[3],
    float magnetometerRawValues[3],
    float xAngleAfterFusion[3],
    float yAngleAfterFusion[3],
    float timeDelta)
{
    accX = accelerometerRawValues[0];
    accY = accelerometerRawValues[1];
    accZ = accelerometerRawValues[2];
    gyroX = gyroscopeRawValues[0];
    gyroY = gyroscopeRawValues[1];
    gyroZ = gyroscopeRawValues[2];

    float dt = timeDelta;

// Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
// atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
// It is then converted from radians to degrees
#ifdef RESTRICT_PITCH  // Eq. 25 and 26
    float roll = atan2f(accY, accZ) * RAD_TO_DEG_FLOAT;
    float pitch = atanf(-accX / sqrtf(accY * accY + accZ * accZ)) * RAD_TO_DEG_FLOAT;
#else  // Eq. 28 and 29
    float roll = atanf(accY / sqrtf(accX * accX + accZ * accZ)) * RAD_TO_DEG_FLOAT;
    float pitch = atan2f(-accX, accZ) * RAD_TO_DEG_FLOAT;
#endif

    float gyroXrate = gyroX / 131.0F;  // Convert to deg/s
    float gyroYrate = gyroY / 131.0F;  // Convert to deg/s

#ifdef RESTRICT_PITCH
    // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
    if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90))
    {
        kalmanX.setAngle(roll);
        compAngleX = roll;
        kalAngleX = roll;
        gyroXangle = roll;
    }
    else
        kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt);  // Calculate the angle using a Kalman filter

    if (abs(kalAngleX) > 90)
        gyroYrate = -gyroYrate;  // Invert rate, so it fits the restriced accelerometer reading
    kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);
#else
    // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
    if ((pitch < -90 && kalAngleY > 90) || (pitch > 90 && kalAngleY < -90))
    {
        kalmanY.setAngle(pitch);
        compAngleY = pitch;
        kalAngleY = pitch;
        gyroYangle = pitch;
    }
    else
        kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);  // Calculate the angle using a Kalman filter

    if (abs(kalAngleY) > 90)
        gyroXrate = -gyroXrate;                         // Invert rate, so it fits the restriced accelerometer reading
    kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt);  // Calculate the angle using a Kalman filter
#endif


#ifndef CALCULATE_GYRO_ANGLE_USING_THE_UNBIASED_RATE
    gyroXangle += gyroXrate * dt;
    gyroYangle += gyroYrate * dt;
#else
    gyroXangle += kalmanX.getRate() * dt;
    gyroYangle += kalmanY.getRate() * dt;
#endif

    compAngleX = complementaryWeight * (compAngleX + gyroXrate * dt) + (1.0F - complementaryWeight) * roll;
    compAngleY = complementaryWeight * (compAngleY + gyroYrate * dt) + (1.0F - complementaryWeight) * pitch;

    // Reset the gyro angle when it has drifted too much
    if (gyroXangle < -180 || gyroXangle > 180)
        gyroXangle = kalAngleX;
    if (gyroYangle < -180 || gyroYangle > 180)
        gyroYangle = kalAngleY;

    xAngleAfterFusion[0] = 0; //gyroXangle;
    xAngleAfterFusion[1] = compAngleX;
    xAngleAfterFusion[2] = kalAngleX;
    yAngleAfterFusion[0] = 0; //gyroYangle;
    yAngleAfterFusion[1] = compAngleY;
    yAngleAfterFusion[2] = kalAngleY;
}
