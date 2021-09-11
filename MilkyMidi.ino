#include "config.h"
#include <MIDI.h>
#include "MilkyMidiTypes.hpp"
#include "LedController.h"
#include "PresetController.hpp"
#include "LooperController.hpp"
#include "ButtonController.hpp"
#include "ProgramManager.hpp"
#include "EffectController.hpp"
#include "Button.h"
#include <Callback.h>

Button buttons[FOOTSWITCH_COUNT] = {
    Button(2, 0),
    Button(3, 1),
    Button(4, 2),
    Button(5, 3),
    Button(0, 4),
    Button(1, 5),
};

MIDI_CREATE_DEFAULT_INSTANCE();

LedController ledController = LedController();
LooperController looperController = LooperController(&MIDI, MIDI_CHANNEL, &ledController.ledSignal);
PresetController presetController = PresetController(&MIDI, BANK_SIZE, MIDI_CHANNEL, &ledController.ledSignal);
EffectController effectController = EffectController(&MIDI, MIDI_CHANNEL, &ledController.ledSignal);
ButtonController buttonController = ButtonController();

ProgramManager programManager = ProgramManager(
    &presetController,
    &effectController,
    &looperController);

void setup()
{
#if defined(DEBUG)
  Serial.begin(19200); // DEBUG ONLY
  Debug.setDebugLevel(DBG_VERBOSE);

  Debug.print(DBG_VERBOSE, "Setup");
#else
  Serial.begin(31250); // MIDI
  Debug.setDebugLevel(DBG_NONE);

#endif

  ledController.Setup();
  buttonController.Setup(buttons);
  programManager.LinkButtons(buttons);
}

void loop()
{
  buttonController.Poll();
}
