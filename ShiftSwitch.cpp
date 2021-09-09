
#include "ShiftSwitch.h"
// #include <ShiftSwitch.h>

ShiftSwitch::ShiftSwitch(int shiftRegisterIndex, int program) {
  this->shiftRegisterIndex = shiftRegisterIndex;
  this->program = program;
  this->timeSinceButtonDown = millis();
  this->timeSinceButtonUp = millis();
}

ShiftSwitch::~ShiftSwitch() {
}


// Dirty hack to compare..
int ShiftSwitch::getProgramNumber() {
  return this->program;
}

void ShiftSwitch::connectButtonLongPressCallback(CallbackFunction cb) {
  this->longPressCallback = cb;
}

void ShiftSwitch::connectButtonPressCallback(CallbackFunction cb) {
  this->pressCallback = cb;
}

void ShiftSwitch::connectButtonDownCallback(CallbackFunction cb) {
  this->buttonDownCallback = cb;
}

void ShiftSwitch::Poll(byte data) {
  bool active = bitRead(data, this->shiftRegisterIndex);

  // // If the switch changed, due to noise or pressing:
  // if (active != this->previousButtonState) {
  //   // reset the debouncing timer
  //   this->lastDebounceTime = millis();
  // }

  // if ((millis() - this->lastDebounceTime) > this->debounceDelay) {

    if (active) {
      this->timeSinceButtonDown = millis();
    } else {
      this->timeSinceButtonUp = millis();
    }

    const long timePassed = this->timeSinceButtonDown - this->timeSinceButtonUp;

    // Button down
    if (active && !this->previousButtonState && (timePassed <= this->debounceDelay) ) {

      if (this->buttonDownCallback != nullptr) {
        this->buttonDownCallback(this->program);
      }

      this->buttonActive = true;
      this->timeSinceButtonDown = millis();

      //     Serial.print("shiftRegisterIndex, ");
      // Serial.print(this->shiftRegisterIndex);
      // Serial.print(", program: ");
      // Serial.println(this->program);
    }
    // PRESSED
    else if (!active && this->previousButtonState && (timePassed <= this->timeTreshold) && (timePassed <= this->debounceDelay) && this->buttonActive) {

      if (this->pressCallback != nullptr) {

        this->pressCallback(this->program);
        // Serial.println("PRESSED");
      }

      this->buttonActive = false;
    }
    // long press
    else if (timePassed >= this->timeTreshold && this->buttonActive) {
      if (this->longPressCallback != nullptr) {
        this->longPressCallback(this->program);
        // Serial.println("LONGPRESS");
      }
      this->buttonActive = false;
    }

    this->previousButtonState = active;
  // }
}