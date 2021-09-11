#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include "Arduino.h"

class LedController
{
public:
  LedController() {}

  ~LedController() {}

  void Setup()
  {
    // 74HC595 pins
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    writeLedState(0);
}

  void writeLedState(byte leds)
  {
    // ST_CP LOW to keep LEDs from changing while reading serial data
    digitalWrite(latchPin, LOW);
    // Shift out the bits
    shiftOut(dataPin, clockPin, MSBFIRST, leds);
    // ST_CP HIGH change LEDs
    digitalWrite(latchPin, HIGH);
  }

private:
  // Define Connections to 74HC595
  const int latchPin = 10; // ST_CP pin 12
  const int clockPin = 11; // SH_CP pin 11
  const int dataPin = 12;  // DS pin 14
};

#endif