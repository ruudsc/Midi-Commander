#include <MIDI.h>
#include "ShiftSwitch.h"

/**
 TODO:, reset looper when exiting mode.
*/

// #define DEBUG TRUE

// Define Connections to 74HC165
int load = 7;            // PL pin 1
int clockEnablePin = 4;  // CE pin 15
int dataIn = 5;          // Q7 pin 7
int clockIn = 6;         // CP pin 2

// Define Connections to 74HC595
const int latchPin = 10;  // ST_CP pin 12
const int clockPin = 11;  // SH_CP pin 11
const int dataPin = 12;   // DS pin 14

MIDI_CREATE_DEFAULT_INSTANCE();

const int MODE_SWITCH_PIN = 5;
const int MODE_PRESET = 0;
const int MODE_EFFECTS = 1;
const int MODE_LOOPER = 2;

int currentMode = 0;
int programActive = 1;

bool fxToggleMap[8] = { true, false, false, false, false, false, false, false };

ShiftSwitch switches[8] = {
  ShiftSwitch(4, 0),
  ShiftSwitch(5, 1),
  ShiftSwitch(0, 2),
  ShiftSwitch(1, 3),
  ShiftSwitch(2, 4),
  ShiftSwitch(3, 5),
  ShiftSwitch(6, 6),
  ShiftSwitch(7, 7),
};

void setup() {
#if defined(DEBUG)
  Serial.begin(19200);  // DEBUG ONLY
#else
  Serial.begin(31250);  // MIDI
#endif

  // 74HC165 pins
  pinMode(load, OUTPUT);
  pinMode(clockEnablePin, OUTPUT);
  pinMode(clockIn, OUTPUT);
  pinMode(dataIn, INPUT);

  // 74HC595 pins
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  setLedState();

  // first 7 switches are bank changes
  for (int i = 0; i < 8; i++) {
    switches[i].connectButtonDownCallback(onButtonDown);
  }

  switches[MODE_SWITCH_PIN].connectButtonDownCallback(onModePressed);
  switches[MODE_SWITCH_PIN].connectButtonLongPressCallback(onModeHold);
  // switches[MODE_SWITCH_PIN].connectButtonPressCallback(onModePressed);
}

void empty() {}

void onModeHold() {
  currentMode = MODE_LOOPER;
#if defined(DEBUG)
  Serial.print("current mode: ");
  Serial.println(currentMode);
#endif
}

void onModePressed() {
  if (currentMode == 0) {
    currentMode = MODE_EFFECTS;
  } else {
    currentMode = MODE_PRESET;
  }

#if defined(DEBUG)
  Serial.print("current mode: ");
  Serial.println(currentMode);
#endif
}

/**
 Hack to prevent looper commands from being spammed
*/
// int previousLooperIndex = -1;

void onButtonDown(int index) {
#if defined(DEBUG)
  Serial.print("onButtonDown: ");
  Serial.println(index);
#endif

  if (currentMode == MODE_PRESET) {
    programActive = index;
    MIDI.sendProgramChange(index, 1);
  } else if (currentMode == MODE_EFFECTS) {
    midiEffectToggle(index);
  } else if (currentMode == MODE_LOOPER) {

    // if(previousLooperIndex != index)  {
    sendLooperCommands(index);
    // }


    // previousLooperIndex = index;
  }
}

void midiProgramChange(int program) {
  /*
      Loading a Setlist Remotely
    From your MIDI controller device, send a Bank Change CC32 (LSB) message with a value
    of 0 (for Setlist 1), 1 (for Setlist 2), etc., followed by a Program Change message (Value
    0-127 for Preset 01A—32D) for the desired preset within the setlist.
  */
  MIDI.sendProgramChange(program, 1);
}

void midiEffectToggle(int index) {
  const int offset = 5;  // send from 3 and up to avoid reserved HX stomp functions
  fxToggleMap[index] = !fxToggleMap[index];

  int ccValue = fxToggleMap[index] ? 127 : 0;
  MIDI.sendControlChange(index + offset, ccValue, 1);
}

  bool looperToggleMap[8] = { true, false, false, false, false, false, false, false };

void sendLooperCommands(int index) {
  // Half 1 Switch Looper Full/Half Speed
  //   MIDI.sendControlChange(65, toggle, 1);

  // Full/Half S
  // MIDI.sendControlChange(66, toggle, 1);


  int toggle = looperToggleMap[index] ? 120 : 20;

#if defined(DEBUG)
  Serial.print("Toggle: ");
  Serial.print(toggle);
  Serial.print(", Looper command: ");
  Serial.println(index);
#endif

  switch (index) {
    case 0:

      MIDI.sendControlChange(61, toggle, 1);  // play/stp[]
      break;
    case 1:
    looperToggleMap[0] = true; // Let button 1 keep playing
      MIDI.sendControlChange(60, 120, 1);  // record
      break;
    case 2:
    looperToggleMap[0] = true; // Let button 1 keep playing
      MIDI.sendControlChange(60, 2, 1);  // overdub
      break;
    case 3:
      MIDI.sendControlChange(63, 120, 1);  // undo/redo
    case 4:
      MIDI.sendControlChange(65, toggle, 1);  // reversed
      break;
    default:
      break;
  }

    looperToggleMap[index] = !looperToggleMap[index];

}

void readFootswitches() {
  // Write pulse to load pin
  digitalWrite(load, LOW);
  delayMicroseconds(5);
  digitalWrite(load, HIGH);
  delayMicroseconds(5);

  // Get data from 74HC165
  digitalWrite(clockIn, HIGH);
  digitalWrite(clockEnablePin, LOW);
  byte incomingBytes = shiftIn(dataIn, clockIn, MSBFIRST);
  digitalWrite(clockEnablePin, HIGH);



  for (int i = 0; i < 8; i++) {
    switches[i].Poll(~incomingBytes);
  }
}

void setLedState() {
  byte ledState = B00000000;

  if (currentMode == MODE_PRESET) {
    int ledIndex = programActive + 1;
    bitSet(ledState, ledIndex);
  }

  if (currentMode == MODE_LOOPER) {
    ledState = B11111111;
  }

  if (currentMode == MODE_EFFECTS) {
    for (int i = 0; i < 8; i++) {
      if (fxToggleMap[i]) {
        bitSet(ledState, i + 1);
      }
    }
    bitSet(ledState, 6);
  }

  // ST_CP LOW to keep LEDs from changing while reading serial data
  digitalWrite(latchPin, LOW);
  // Shift out the bits
  shiftOut(dataPin, clockPin, MSBFIRST, ledState);
  // ST_CP HIGH change LEDs
  digitalWrite(latchPin, HIGH);
}

void loop() {
  readFootswitches();
  setLedState();

  delay(30);
  // #if defined(DEBUG)
  // Slow down so Serial will chill
  // delay(50);
  // #endif
}
