#include <Arduino.h>
#include <protocol.h>
#include <MPU9250.h>


// an MPU9250 object with the MPU-9250 sensor on SPI bus 0 and chip select pin 10
MPU9250 IMU(SPI, 10);
int status;


void setup()
{
    // serial to display data
    Serial.begin(115200);
    while (!Serial)
    {
    }

    // start communication with IMU
    status = IMU.begin();
    if (status < 0)
    {
        Serial.println("IMU initialization unsuccessful");
        Serial.println("Check IMU wiring or try cycling power");
        Serial.print("Status: ");
        Serial.println(status);
        while (1)
        {
        }
    }
}

#define BUFFER_SIZE (64U)
uint8_t buffer[BUFFER_SIZE];
uint8_t bufferIndex = 0U;

void processIncomingPacket(const uint8_t *packet)
{
    const PacketHeader *header = (const PacketHeader *)packet;
    switch (header->payloadId)
    {
        case PAYLOAD_PING:
        {
            const PingPacket * pingPacket = (const PingPacket *) packet;

            PongPacket pongPacket = {0};
            pongPacket.payload.pong = pingPacket->payload.ping * pingPacket->payload.ping;

            setHeader((uint8_t*)&pongPacket, PAYLOAD_PONG);
            Serial.write((uint8_t *)&pongPacket, sizeof(PongPacket));
        }
        default:
            return;
    }
}

void loop()
{
    if (Serial.available() > 0)
    {
        uint8_t incomingByte = (uint8_t)Serial.read();

        processIncomingByte(buffer, &bufferIndex, BUFFER_SIZE, (uint32_t)millis(), incomingByte, processIncomingPacket);
    }

    if (millis() % 100U == 0U)
    {
        SensorPacket sensorPacket = {0};

        IMU.readSensor();

        sensorPacket.payload.accelerometerMSS[0] = IMU.getAccelX_mss();
        sensorPacket.payload.accelerometerMSS[1] = IMU.getAccelY_mss();
        sensorPacket.payload.accelerometerMSS[2] = IMU.getAccelZ_mss();
        sensorPacket.payload.gyroscopeRads[0] = IMU.getGyroX_rads();
        sensorPacket.payload.gyroscopeRads[1] = IMU.getGyroY_rads();
        sensorPacket.payload.gyroscopeRads[2] = IMU.getGyroZ_rads();
        sensorPacket.payload.magnetometerMicroT[0] = IMU.getMagX_uT();
        sensorPacket.payload.magnetometerMicroT[1] = IMU.getMagY_uT();
        sensorPacket.payload.magnetometerMicroT[2] = IMU.getMagZ_uT();
        sensorPacket.payload.temperatureCelcius = IMU.getTemperature_C();

        setHeader((uint8_t *)&sensorPacket, PAYLOAD_SENSOR);

        Serial.write((uint8_t *)&sensorPacket, sizeof(SensorPacket));
    }
}
