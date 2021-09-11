#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include "Arduino.h"
#include <Callback.h>

class LedController
{
public:
  LedController()
  {
    auto writeLedStateSlot = MethodSlot<LedController, byte>(this, &LedController::writeLedState);
    ledSignal.attach(writeLedStateSlot);
  }

  ~LedController() {}

  void Setup()
  {
    // 74HC595 pins
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    // byte ledState = B01010101;
    byte ledState = B11111111;
    writeLedState(ledState);
  }

  void writeLedState(byte leds)
  {

    Debug.print(DBG_VERBOSE, "setting leds: %hhx", leds);

    // ST_CP LOW to keep LEDs from changing while reading serial data
    digitalWrite(latchPin, LOW);
    // Shift out the bits
    shiftOut(dataPin, clockPin, MSBFIRST, leds);
    // ST_CP HIGH change LEDs
    digitalWrite(latchPin, HIGH);
  }

  Signal<byte> ledSignal;

private:
  // Define Connections to 74HC595
  const int latchPin = 10; // ST_CP pin 12
  const int clockPin = 11; // SH_CP pin 11
  const int dataPin = 12;  // DS pin 14
};

#endif