#include "SoftwareSerial.h"
#include "biomechanical_state.h"
#include "xinput_loop.h"
#include <Arduino.h>
#include <xinput_setup.h>

#define LED_PROMICRO 17

#define XINPUT_INTERVAL 10

uint32_t systemClockMillis = 0U;

char buffer[256];
int bufferIndex = 0;

int leftFoot[3];
int rightFoot[3];

void readSerial()
{
    while (Serial1.available())
    {
        buffer[bufferIndex] = Serial1.read();
        if (buffer[bufferIndex] == '\n')
        {
            sscanf(buffer, "%d %d %d %d %d %d", &leftFoot[0], &leftFoot[1], &leftFoot[2], &rightFoot[0], &rightFoot[1], &rightFoot[2]);
            bufferIndex = 0U;
        }
        else if (bufferIndex < 255)
        {
            bufferIndex++;
        }
        else
        {
            bufferIndex = 0U;
        }
    }
}

void setup()
{
    pinMode(LED_PROMICRO, OUTPUT);
    Serial1.begin(115200);
    xinput_setup();
}

void loop()
{
    static uint32_t xinputMillis = 0U;
    systemClockMillis = millis();

    readSerial();

    if (systemClockMillis - xinputMillis > XINPUT_INTERVAL)
    {
        xinput_loop(leftFoot, rightFoot);
        xinputMillis = systemClockMillis;
    }
}
