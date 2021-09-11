#ifndef BUTTON_CONTROLLER
#define BUTTON_CONTROLLER

#include "Arduino.h"
#include "Button.h"
#include "MilkyMidiTypes.hpp"
#include <Arduino_DebugUtils.h>

class ButtonController
{
public:
  ButtonController()
  {
  }

  ~ButtonController() {}

  void Setup(Button buttons[])
  {
    for (int i = 0; i < FOOTSWITCH_COUNT; i++)
    {
      this->buttons[i] = &buttons[i];
    }

    // 74HC165 pins
    pinMode(load, OUTPUT);
    pinMode(clockEnablePin, OUTPUT);
    pinMode(clockIn, OUTPUT);
    pinMode(dataIn, INPUT);
  }

  void Poll()
  {

    byte buttonStates = this->ReadShiftRegister();

    // TODO: change button<> to button<T>
    for (int i = 0; i < FOOTSWITCH_COUNT; i++)
    {
      bool buttonState = bitRead(buttonStates, i);
      buttons[i]->Update(buttonState);
    }
  }

private:
  // Define Connections to 74HC165
  const int load = 7;           // PL pin 1
  const int clockEnablePin = 4; // CE pin 15
  const int dataIn = 5;         // Q7 pin 7
  const int clockIn = 6;        // CP pin 2

  Button *buttons[FOOTSWITCH_COUNT];

  byte ReadShiftRegister()
  {
    digitalWrite(load, LOW);
    delayMicroseconds(5);
    digitalWrite(load, HIGH);
    delayMicroseconds(5);

    // Get data from 74HC165
    digitalWrite(clockIn, HIGH);
    digitalWrite(clockEnablePin, LOW);
    byte incomingBytes = shiftIn(dataIn, clockIn, MSBFIRST);
    digitalWrite(clockEnablePin, HIGH);

    return ~incomingBytes;
  }
};

#endif