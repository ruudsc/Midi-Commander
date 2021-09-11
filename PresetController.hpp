#ifndef PRESET_CONTROLLER_H
#define PRESET_CONTROLLER_H

#include "Arduino.h"
#include "MilkyMidiTypes.hpp"
#include <MIDI.h>

using MidiInterface = MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>>;

class PresetController
{
public:
  PresetController(MidiInterface *midi, midi::DataByte bankSize, midi::Channel channel, Signal<byte> *ledSignal)
  {
    this->midi = midi;
    this->bankSize = 5;
    this->channel = channel;
    this->ledSignal = ledSignal;
  }

  ~PresetController() {}

  void sendProgramChange()
  {
    const auto programNumber = (bank * bankSize) + bankItem;
    this->midi->sendProgramChange(programNumber, 1);
    updateLedState();
    Debug.print(DBG_VERBOSE, "PresetController::sendProgramChange programNumber: %d", programNumber);
    Debug.print(DBG_VERBOSE, "bankItem: %d, bankSize: %d", bankItem, bankSize);
  }

  void onModeEnter()
  {
    updateLedState();
  }

  void onBankItemSelect(midi::DataByte index)
  {
    this->bankItem = index;
    sendProgramChange();
  }

  void onBankIncrement()
  {
    Debug.print(DBG_VERBOSE, "Bank Increment");
    if (bank < 126)
      this->bank++;
    sendProgramChange();
  }

  void onBankDecrement()
  {
    Debug.print(DBG_VERBOSE, "Bank decrement");

    if (bank > 0)
      this->bank--;
    sendProgramChange();
  }

  void updateLedState()
  {
    byte ledState = B00000000;
    bitSet(ledState, this->bankItem + 1);
    ledSignal->fire(ledState);
  }

private:
  MidiInterface *midi;
  midi::Channel channel = 1;
  midi::DataByte currentProgram = 1;
  midi::DataByte bankItem = 1;
  midi::DataByte bank = 0;
  midi::DataByte bankSize = 5;
  Signal<byte> *ledSignal;
};

#endif