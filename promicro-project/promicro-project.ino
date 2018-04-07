#include <Arduino.h>

#define LED_PROMICRO 17

void setup() {
  pinMode(LED_PROMICRO, OUTPUT);
}

void loop() {
  digitalWrite(LED_PROMICRO, HIGH);
  delay(100);
  digitalWrite(LED_PROMICRO, LOW);
  delay(100);
}

