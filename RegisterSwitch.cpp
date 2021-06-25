#include "RegisterSwitch.hh"

void RegisterSwitch::Poll(byte state) {
  bool currentSwitchState = bitRead(state, nShiftIndex);
  bool stateChanged = currentSwitchState != previousSwitchState;
  unsigned long elapsedTime = millis() - presseDownTimestamp;

  RegisterSwitchData data = {};
  data.shiftIndex = nShiftIndex;
  data.state = currentSwitchState;
  data.elapsedTime = elapsedTime;


  if(currentSwitchState && stateChanged && currentSwitchState) {
    presseDownTimestamp = millis();

    if(onPressCallback) {
      (*onPressCallback)(data); 

    }
  } 


  if(!previousHoldState && currentSwitchState && (elapsedTime > longPressTimeTreshold)) {
    previousHoldState = true;
    if(onLongPressCallback) (*onLongPressCallback)(data); 
  } else {
    previousHoldState = false;
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


