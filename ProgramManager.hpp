#ifndef PROGRAM_MANAGER_H
#define PROGRAM_MANAGER_H

#include "Arduino.h"
#include "MilkyMidiTypes.h"
#include "Button.h"
#include "PresetController.hpp"
#include "EffectController.hpp"
#include "LooperController.hpp"

#define OPTION_BUTTON 5

class ProgramManager
{
public:
    ProgramManager(PresetController *preset, EffectController *effect, LooperController *looper)
    {
        this->preset = preset;
        this->effects = effect;
        this->looper = looper;
    }

    ~ProgramManager() {}

    void LinkButtons(Button<> buttons[])
    {
        auto buttonDownSlot = MethodSlot<ProgramManager, ButtonPayload>(this, &ProgramManager::onFootswitchDown);
        auto modeButtonDown = MethodSlot<ProgramManager, ButtonPayload>(this, &ProgramManager::onOptionPressed);
        auto modeButtonHold = MethodSlot<ProgramManager, ButtonPayload>(this, &ProgramManager::onOptionHold);

        for (int i = 0; i < FOOTSWITCH_COUNT; i++)
        {
            if (i != OPTION_BUTTON)
                buttons[i].buttonDownSignal.attach(buttonDownSlot);
        }

        // // Remove button down action for mode
        buttons[OPTION_BUTTON].buttonPressedCallSignal.attach(modeButtonDown);
        buttons[OPTION_BUTTON].buttonHoldSignal.attach(modeButtonHold);
    }

    void onFootswitchDown(ButtonPayload payload)
    {

        switch (currentMode)
        {
        case SwitchModes::PRESET:
            preset->onBankItemSelect(payload.index);
            break;
        case SwitchModes::EFFECTS:
            effects->onToggleEffect(payload);
            break;
        case SwitchModes::LOOPER:
            looper->handleIncomingLooperCommand(payload.index);
            break;
        }
    }

    void onOptionPressed(ButtonPayload payload)
    {

        ToggleModes(payload);
    }

    void onOptionHold(ButtonPayload payload)
    {

        GotoLooperMode();
    }

    void ResetMode()
    {
        currentMode = SwitchModes::PRESET;
    }

    void ToggleModes(ButtonPayload payload)
    {
        const auto nextMode = (currentMode + 1) % static_cast<int>(SwitchModes::NUMBER_OF_MODES);
        currentMode = static_cast<SwitchModes>(nextMode);
    }

    void GotoPresetMode()
    {
        currentMode = SwitchModes::PRESET;
    }

    void GotoEffectMode()
    {
        currentMode = SwitchModes::EFFECTS;
    }

    void GotoLooperMode()
    {
        currentMode = SwitchModes::LOOPER;
    }

private:
    SwitchModes currentMode = SwitchModes::PRESET;
    PresetController *preset;
    EffectController *effects;
    LooperController *looper;
};

#endif