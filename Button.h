#ifndef BUTTON_H
#define BUTTON_H

#include "MilkyMidiTypes.hpp"
#include "ButtonEvent.h"
#include <Arduino_DebugUtils.h>
#include <Callback.h>

class Button
{
public:
  Button(byte index, byte shiftRegisterIndex)
  {
    this->index = index;
    this->shiftRegisterIndex = shiftRegisterIndex;

    Debug.print(DBG_VERBOSE, "Button instance");
  };

  ~Button(){};

  Signal<ButtonEvent> buttonDownSignal;
  Signal<ButtonEvent> buttonUpSignal;
  Signal<ButtonEvent> buttonPressedCallSignal;
  Signal<ButtonEvent> buttonHoldSignal;

  void Update(bool state)
  {
    const auto debouncedInput = this->Debounce(state);

    const unsigned long timePassed = this->timeSinceButtonDown - this->timeSinceButtonUp;

    if (debouncedInput)
      this->timeSinceButtonDown = millis();
    else
      this->timeSinceButtonUp = millis();

    // DOWN
    if (debouncedInput && !previousButtonState)
    {
      ButtonEvent payload = {
        index : this->index,
        shiftRegisterIndex : this->shiftRegisterIndex,
        eventType : ButtonEventType::DOWN
      };

      this->buttonDownSignal.fire(payload);
      holdEventActive = false;
    }

    // UP
    if (!debouncedInput && previousButtonState)
    {
      ButtonEvent payload = {
        index : this->index,
        shiftRegisterIndex : this->shiftRegisterIndex,
        eventType : ButtonEventType::UP
      };
      this->buttonUpSignal.fire(payload);
      // Debug.print(DBG_VERBOSE, "UP: %d, prevState %d, timePassed: %d", debouncedInput, previousButtonState, timePassed);
    }

    // PRESSED
    if (!debouncedInput && previousButtonState && (timePassed <= longPressTreshold))
    {
      ButtonEvent payload = {
        index : this->index,
        shiftRegisterIndex : this->shiftRegisterIndex,
        eventType : ButtonEventType::PRESSED
      };
      this->buttonPressedCallSignal.fire(payload);
      // Debug.print(DBG_VERBOSE, "PRESS: %d, prevState %d, timePassed: %d", debouncedInput, previousButtonState, timePassed);
    }

    // HOLD
    if (debouncedInput && (int)timePassed > 0 && timePassed > this->longPressTreshold && !holdEventActive)
    {
      ButtonEvent payload = {
        index : this->index,
        shiftRegisterIndex : this->shiftRegisterIndex,
        eventType : ButtonEventType::HOLD
      };

      this->buttonHoldSignal.fire(payload);
      holdEventActive = true;
      // Debug.print(DBG_VERBOSE, "HOLD: button: %d, prevState %d, timePassed: %d", debouncedInput, previousButtonState, timePassed);
    }

    this->previousButtonState = debouncedInput;
  }

private:
  unsigned long debounceTreshold = 50;
  unsigned long prevDebounceTime = millis();

  unsigned long timeSinceButtonDown = millis();
  unsigned long timeSinceButtonUp = millis();
  unsigned long longPressTreshold = 1000;

  bool previousButtonState;
  bool debouncedState = false;
  bool previousDebouncedState = false;
  bool holdEventActive = false;

  byte index;
  ;
  byte shiftRegisterIndex;

  bool Debounce(bool rawState)
  {

    // If the switch changed, due to noise or pressing:
    if (rawState != this->previousDebouncedState)
      this->prevDebounceTime = millis();

    if ((millis() - this->prevDebounceTime) > this->debounceTreshold)
      this->debouncedState = rawState;

    // Debug.print(DBG_VERBOSE, "raw %d", rawState);
    this->previousDebouncedState = rawState;

    return debouncedState;
  }
};

#endif