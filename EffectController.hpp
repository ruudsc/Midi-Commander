#ifndef EFFECT_CONTROLLER_H
#define EFFECT_CONTROLLER_H

#include "Arduino.h"
#include "MilkyMidiTypes.hpp"
#include "ButtonEvent.h"

#define HELIX_BLOCK_COUNT 8
#define HELIX_CC_OFFSET 4

class EffectController
{
public:
    EffectController(MidiInterface *midi, midi::Channel channel, Signal<byte> *ledSignal)
    {
        this->midi = midi;
        this->channel = channel;
        for (int i = 0; i < HELIX_BLOCK_COUNT; i++)
            effectStates[i] = false;

        this->ledSignal = ledSignal;
    }

    ~EffectController() {}

    void onModeEnter()
    {
        updateLedState();
    }

    void onToggleEffect(ButtonEvent payload)
    {
        Debug.print(DBG_VERBOSE, "EffectController::onToggleEffect programNumber: %d", payload.index);

        // send from 4 and up to avoid reserved HX stomp functions
        const int ccNumber = payload.index + HELIX_CC_OFFSET;
        const int ccValue = effectStates[payload.index] ? MIDI_HIGH : MIDI_LOW;

        midi->sendControlChange(ccNumber, ccValue, channel);
        effectStates[payload.index] = !effectStates[payload.index];

        updateLedState();
    }

    updateLedState()
    {
        byte ledState = B01000000;

        for (int i = 0; i < 8; i++)
        {
            if (effectStates[i])
            {
                bitSet(ledState, i + 1);
            }
        }

        ledSignal->fire(ledState);
    }

private:
    bool effectStates[HELIX_BLOCK_COUNT];
    MidiInterface *midi;
    midi::Channel channel;
    Signal<byte> *ledSignal;
};

#endif