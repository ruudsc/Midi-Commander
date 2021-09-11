#ifndef MILKY_MIDI_TYPES_H
#define MILKY_MIDI_TYPES_H
#include "config.h"
#include <MIDI.h>

using MidiInterface = MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>>;

#define MIDI_HIGH 127
#define MIDI_LOW 0
#define FOOTSWITCH_COUNT 6

enum class SwitchModes
{
  PRESET = 0,
  EFFECTS,
  LOOPER,
  NUMBER_OF_MODES
};

#endif