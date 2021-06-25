
#ifndef REGISTER_SWITCH_H
#define REGISTER_SWITCH_H

#include "arduino.h";

struct RegisterSwitchData {
  int shiftIndex;
  bool state;
  unsigned long elapsedTime; // time since pressed
};


using RegisterSwitchCallback = void (*)(RegisterSwitchData);

class RegisterSwitch {
public:
  RegisterSwitch(int shiftIndex)
    : nShiftIndex(shiftIndex),
      presseDownTimestamp(0),
      longPressTimeTreshold(300),
      onPressCallback(nullptr),
      onLongPressCallback(nullptr)
      {}
  ~RegisterSwitch(){};

  void Poll(byte state);

  void StartTimer();
  void ResetTimer();
  

  void setOnDownCallback(RegisterSwitchCallback callback);
  void setOnUpCallback(RegisterSwitchCallback callback);
  void setOnLongPressCallback(RegisterSwitchCallback callback);


  // Start timer
  // reset timer
  //if timer greater 300ms
  // if onpressed


private:
  int nShiftIndex;
  bool previousSwitchState = false;
  bool previousHoldState = false;
  unsigned long presseDownTimestamp;
  int longPressTimeTreshold;

  RegisterSwitchCallback onPressCallback;
  RegisterSwitchCallback onLongPressCallback;
  void placeholder(){};
};

#endif