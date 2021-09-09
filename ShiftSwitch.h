#include "Arduino.h"

typedef void (*CallbackFunction)(int);

class ShiftSwitch {
public:
  ShiftSwitch(int shiftRegisterIndex, int program);
  ~ShiftSwitch();
  void Poll(byte shiftRegisterData);
  void connectButtonLongPressCallback(CallbackFunction cb);
  void connectButtonPressCallback(CallbackFunction cb);
  void connectButtonDownCallback(CallbackFunction cb);
  int getProgramNumber();

private:
  int shiftRegisterIndex;
  int program;

  void dummyFunc(int dummy);

  CallbackFunction pressCallback;
  CallbackFunction longPressCallback;
  CallbackFunction buttonDownCallback;

  unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
  const unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers

  unsigned long timeSinceButtonDown;
  unsigned long timeSinceButtonUp;
  unsigned long lastActionTimestamp;
  const unsigned int timeTreshold = 550;
  const unsigned int debounceTreshold = 5;
  bool previousButtonState;
  bool buttonActive = false;
};