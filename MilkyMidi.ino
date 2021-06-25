#include <MIDI.h>
#include "RegisterSwitch.hh";

// Define Connections to 74HC165
int load = 7;            // PL pin 1
int clockEnablePin = 4;  // CE pin 15
int dataIn = 5;          // Q7 pin 7
int clockIn = 6;         // CP pin 2

// Define Connections to 74HC595
const int latchPin = 10;  // ST_CP pin 12
const int clockPin = 11;  // SH_CP pin 11
const int dataPin = 12;   // DS pin 14

bool bankShift = false;


MIDI_CREATE_DEFAULT_INSTANCE();

bool leds[8] = { false, false, false, false, false, false, false, false };
const int ledMap[8] = { 1, 2, 3, 4, 5, 6, 7, 0 };

bool footswitches[8] = { false, false, false, false, false, false, false, false };
const int footswitchMap[8] = { 2, 3, 4, 5, 0, 1, 6, 7 };

int lastProgramActive = 0;
int programActive = 0;

RegisterSwitch switches[8] = {
  RegisterSwitch(2),
  RegisterSwitch(3),
  RegisterSwitch(4),
  RegisterSwitch(5),
  RegisterSwitch(0),
  RegisterSwitch(1),
  RegisterSwitch(6),
  RegisterSwitch(7),

};

void setup() {
  // Serial.begin(31250);  // MIDI
  Serial.begin(4800);  // DEBUG ONLY

  // 74HC165 pins
  pinMode(load, OUTPUT);
  pinMode(clockEnablePin, OUTPUT);
  pinMode(clockIn, OUTPUT);
  pinMode(dataIn, INPUT);

  // 74HC595 pins
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  for (int i = 0; i < 8; i++) {
    switches[i].setOnDownCallback(OnPressed);
  }

  switches[1].setOnLongPressCallback(OnLongPress);
}

byte BoolArrayToByte(bool boolArray[8]) {
  byte result = 0;

  for (int i = 0; i < 8; i++) {
    if (boolArray[i]) {
      result = result | (1 << i);
    }
  }

  return result;
}

void OnPressed(RegisterSwitchData data) {
  Serial.println("---buttonPressed---");
  Serial.print("shiftIndex: ");
  Serial.print(data.shiftIndex);
  Serial.print(", elapsedTime: ");
  Serial.print(data.elapsedTime);
  Serial.print(", state: ");
  Serial.print(data.state);
  Serial.println("");


  for (int i = 0; i < 8; i++) {
    if (footswitchMap[i] == i) {

      Serial.println(footswitchMap[i]);
      // programActive = i;
      // if (bankShift) {
        // programActive = i + 6;
      // }
    }
  }
}

void OnLongPress(RegisterSwitchData data) {
  bankShift = !bankShift;
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
    // int state = bitRead(~incomingBytes, i);
    switches[i].Poll(~incomingBytes);
  }
}

void setLedState() {
  //byte ledState = BoolArrayToByte(footswitches);
  // We don't use pin 0 so shift it
  //ledState = ledState << 1;
  byte ledState = B00000000;
  bitSet(ledState, programActive + 1);

  if (bankShift) {
    bitSet(ledState, 6);
  }

  // ST_CP LOW to keep LEDs from changing while reading serial data
  digitalWrite(latchPin, LOW);

  // Shift out the bits
  shiftOut(dataPin, clockPin, MSBFIRST, ledState);

  // Serial.println(ledState, BIN);

  // ST_CP HIGH change LEDs
  digitalWrite(latchPin, HIGH);
}

void midiProgramChange() {

  /*
      Loading a Setlist Remotely
    From your MIDI controller device, send a Bank Change CC32 (LSB) message with a value
    of 0 (for Setlist 1), 1 (for Setlist 2), etc., followed by a Program Change message (Value
    0-127 for Preset 01A—32D) for the desired preset within the setlist.
  */

  if (programActive != lastProgramActive) {
    MIDI.sendProgramChange(programActive, 1);
  }
}

void loop() {
  readFootswitches();
  setLedState();
  // midiProgramChange();
}
