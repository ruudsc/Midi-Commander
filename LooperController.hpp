#ifndef LOOPER_CONTROLLER
#define LOOPER_CONTROLLER

#include "Arduino.h"
#include <MIDI.h>
#include "MilkyMidiTypes.h"

#define RECORD_OVERDUB 60
#define PLAY_STOP 61
#define PLAY_ONCE 62
#define UNDO_REDO 63
#define FORWARD_REVERSE 65
#define FULL_HALF_SPEED 66

class LooperController {
public:
  LooperController(MidiInterface *midi, midi::Channel channel = 1) {
    this->midi = midi;
    this->channel = channel;
  }

  ~LooperController() {
  }

  void handleIncomingLooperCommand(byte buttonIndex) {
    switch (buttonIndex) {
      case 0:
        PlayStop();
        break;
      case 1:
        Record();
        break;
      case 2:
        Overdub();
        break;
      case 3:
        UndoRedo();
      case 4:
        ForwardReverse();
        break;
      default:
        break;
    }
  }

  void PlayStop() {
    if (PlayStopState)
      Play();
    else
      Stop();

    PlayStopState = !PlayStopState;
  }

  void Play() {
    this->midi->sendControlChange(PLAY_STOP, MIDI_HIGH, this->channel);
  }

  void PlayOnce() {
    this->midi->sendControlChange(PLAY_ONCE, MIDI_HIGH, this->channel);
  }

  void Record() {
    this->midi->sendControlChange(RECORD_OVERDUB, MIDI_HIGH, this->channel);
  }

  void Overdub() {
    this->midi->sendControlChange(RECORD_OVERDUB, MIDI_LOW, this->channel);
  }

  void Stop() {
    this->midi->sendControlChange(PLAY_STOP, MIDI_LOW, this->channel);
  }

  void UndoRedo() {
    this->midi->sendControlChange(UNDO_REDO, MIDI_HIGH, this->channel);
  }

  void Forward() {
    this->midi->sendControlChange(FORWARD_REVERSE, MIDI_LOW, this->channel);
  }

  void Reverse() {
    this->midi->sendControlChange(FORWARD_REVERSE, MIDI_HIGH, this->channel);
  }

  void ForwardReverse() {
    if (ForwardReverseState)
      Forward();
    else
      Reverse();
  }

  void FullSpeed() {
    this->midi->sendControlChange(FORWARD_REVERSE, MIDI_LOW, this->channel);
  }

  void HalfSpeed() {
    this->midi->sendControlChange(FORWARD_REVERSE, MIDI_HIGH, this->channel);
  }

private:
  MidiInterface *midi;
  midi::Channel channel;

  bool PlayStopState = true;
  bool ForwardReverseState = false;
};

#endif