#ifndef EFFECT_CONTROLLER_H
#define EFFECT_CONTROLLER_H

#include "Arduino.h"
#include "MilkyMidiTypes.h"

#define HELIX_BLOCK_COUNT 8
#define HELIX_CC_OFFSET 4

class EffectController
{
public:
    EffectController(MidiInterface *midi, midi::Channel channel)
    {
        this->midi = midi;
        this->channel = channel;
        for (int i = 0; i < HELIX_BLOCK_COUNT; i++)
            effectStates[i] = false;
    }

    ~EffectController() {}

    void onToggleEffect(ButtonPayload payload)
    {
        // send from 4 and up to avoid reserved HX stomp functions
        const int ccNumber = payload.index + HELIX_CC_OFFSET;
        const int ccValue = effectStates[payload.index] ? MIDI_HIGH : MIDI_LOW;

        midi->sendControlChange(ccNumber, ccValue, channel);
        effectStates[payload.index] = !effectStates[payload.index];
    }

private:
    bool effectStates[HELIX_BLOCK_COUNT];
    MidiInterface *midi;
    midi::Channel channel;
};

#endif