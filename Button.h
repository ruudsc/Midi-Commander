#ifndef BUTTON_H
#define BUTTON_H

#include "MilkyMidiTypes.h"
#include <Arduino_DebugUtils.h>
#include <Callback.h>

template <typename T = ButtonPayload>
class Button
{
public:
  Button(T callbackPayload)
  {
    this->callbackPayload = callbackPayload;

    Debug.print(DBG_VERBOSE, "Button instance");
  };

  ~Button(){};

  Signal<T> buttonDownSignal;
  Signal<T> buttonUpSignal;
  Signal<T> buttonPressedCallSignal;
  Signal<T> buttonHoldSignal;

  T callbackPayload;

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
      this->buttonDownSignal.fire(callbackPayload);
      Debug.print(DBG_VERBOSE, "DOWN: %d, prevState %d, timePassed: %d", debouncedInput, previousButtonState, timePassed);
      holdEventActive = false;
    }

    // UP
    if (!debouncedInput && previousButtonState)
    {
      this->buttonUpSignal.fire(callbackPayload);
      Debug.print(DBG_VERBOSE, "UP: %d, prevState %d, timePassed: %d", debouncedInput, previousButtonState, timePassed);
    }

    // PRESSED
    if (!debouncedInput && previousButtonState && (timePassed <= longPressTreshold))
    {
      this->buttonPressedCallSignal.fire(callbackPayload);
      Debug.print(DBG_VERBOSE, "PRESS: %d, prevState %d, timePassed: %d", debouncedInput, previousButtonState, timePassed);
    }

    // HOLD
    if (debouncedInput && (int)timePassed > 0 && timePassed > this->longPressTreshold && !holdEventActive)
    {
      this->buttonHoldSignal.fire(callbackPayload);
      holdEventActive = true;
      Debug.print(DBG_VERBOSE, "HOLD: button: %d, prevState %d, timePassed: %d", debouncedInput, previousButtonState, timePassed);
    }

    this->previousButtonState = debouncedInput;
  }

private:
  unsigned long debounceTreshold = 50;
  unsigned long prevDebounceTime = 0;

  unsigned long timeSinceButtonDown = millis();
  unsigned long timeSinceButtonUp = millis();
  unsigned long longPressTreshold = 1000;

  bool previousButtonState;
  bool debouncedState = false;
  bool previousDebouncedState = false;
  bool holdEventActive = false;

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