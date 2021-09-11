#ifndef PRESET_CONTROLLER_H
#define PRESET_CONTROLLER_H

#include "Arduino.h"
#include "MilkyMidiTypes.h"
#include <MIDI.h>

using MidiInterface = MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>>;

class PresetController
{
public:
  PresetController(MidiInterface *midi, midi::DataByte bankSize = 5, midi::Channel channel = 1)
  {
    this->midi = midi;
    this->bankSize = 5;
    this->channel = channel;
  }

  ~PresetController() {}

  void sendProgramChange()
  {
    const auto programNumber = (bank * bankSize) + bankItem;
    this->midi->sendProgramChange(programNumber, 1);
  }

  void onBankItemSelect(midi::DataByte index)
  {
    this->bankItem = index;
  }

  void onBankIncrement()
  {
    this->bank++;
  }

  void onBankDecrement()
  {
    this->bank--;
  }

private:
  MidiInterface *midi;
  midi::Channel channel = 1;
  midi::DataByte currentProgram;
  midi::DataByte bankItem;
  midi::DataByte bank;
  midi::DataByte bankSize;
};

#endif