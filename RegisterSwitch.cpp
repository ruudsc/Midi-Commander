#include "RegisterSwitch.hh"

void RegisterSwitch::Poll(byte state) {
  bool currentSwitchState = bitRead(state, nShiftIndex);
  bool stateChanged = currentSwitchState != previousSwitchState;
  unsigned long elapsedTime = millis() - presseDownTimestamp;

  RegisterSwitchData data = {};
  data.shiftIndex = nShiftIndex;
  data.state = currentSwitchState;
  data.elapsedTime = elapsedTime;

  if(currentSwitchState && stateChanged) {
    presseDownTimestamp = millis();

    if(onPressCallback) {
      (*onPressCallback)(data); 
    }
  } 


  if(currentSwitchState && elapsedTime > longPressTimeTreshold) {
    if(onLongPressCallback) (*onLongPressCallback)(data); 
  }
  

  previousSwitchState = currentSwitchState;
}


void RegisterSwitch::setOnDownCallback(RegisterSwitchCallback callback) {
  onPressCallback = callback;
}

void RegisterSwitch::setOnLongPressCallback(RegisterSwitchCallback callback) {
  onLongPressCallback = callback;
}
/*
void RegisterSwitch::setOnUpCallback(RegisterSwitchCallback callback) {
  onUpCallback = callback;
}
*/


