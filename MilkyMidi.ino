#include "config.h"
#include <MIDI.h>
#include "MilkyMidiTypes.h"
#include "LedController.h"
#include "PresetController.hpp"
#include "LooperController.hpp"
#include "ButtonController.hpp"
#include "ProgramManager.hpp"
#include "EffectController.hpp"
#include "Button.h"


Button<ButtonPayload> buttons[FOOTSWITCH_COUNT] = {
    Button<ButtonPayload>({index : 0, shiftRegisterIndex : 4}),
    Button<ButtonPayload>({index : 1, shiftRegisterIndex : 5}),
    Button<ButtonPayload>({index : 2, shiftRegisterIndex : 0}),
    Button<ButtonPayload>({index : 3, shiftRegisterIndex : 1}),
    Button<ButtonPayload>({index : 4, shiftRegisterIndex : 2}),
    Button<ButtonPayload>({index : 5, shiftRegisterIndex : 3}),
};

MIDI_CREATE_DEFAULT_INSTANCE();

LedController ledController = LedController();
LooperController looperController = LooperController(&MIDI, MIDI_CHANNEL);
PresetController presetController = PresetController(&MIDI, BANK_SIZE, MIDI_CHANNEL);
ButtonController buttonController = ButtonController();
EffectController effectController = EffectController(&MIDI, MIDI_CHANNEL);

ProgramManager programManager = ProgramManager(
    &presetController,
    &effectController,
    &looperController);

void setup()
{
  Debug.setDebugLevel(DBG_VERBOSE);

#if defined(DEBUG)
  Serial.begin(19200); // DEBUG ONLY
  Debug.print(DBG_VERBOSE, "Setup");
#else
  Serial.begin(31250); // MIDI
#endif

  ledController.Setup();
  buttonController.Setup(buttons);
  programManager.LinkButtons(buttons);
}

void loop()
{

  buttonController.Poll();
}
