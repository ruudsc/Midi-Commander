#ifndef PROGRAM_MANAGER_H
#define PROGRAM_MANAGER_H

#include "Arduino.h"
#include "MilkyMidiTypes.hpp"
#include "Button.h"
#include "PresetController.hpp"
#include "EffectController.hpp"
#include "LooperController.hpp"
#include "config.h"
#include "ButtonEvent.h"

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

    void LinkButtons(Button buttons[])
    {
        auto buttonHoldSlot = MethodSlot<ProgramManager, ButtonEvent>(this, &ProgramManager::onFootswitchHold);
        auto buttonDownSlot = MethodSlot<ProgramManager, ButtonEvent>(this, &ProgramManager::onFootswitchDown);
        auto modeButtonDown = MethodSlot<ProgramManager, ButtonEvent>(this, &ProgramManager::onOptionPressed);
        auto modeButtonHold = MethodSlot<ProgramManager, ButtonEvent>(this, &ProgramManager::onOptionHold);

        for (int i = 0; i < FOOTSWITCH_COUNT; i++)
        {
            if (i != OPTION_BUTTON)
            {
                buttons[i].buttonDownSignal.attach(buttonDownSlot);
                buttons[i].buttonHoldSignal.attach(buttonHoldSlot);
            }
        }

        // // Remove button down action for mode
        buttons[OPTION_BUTTON].buttonPressedCallSignal.attach(modeButtonDown);
        buttons[OPTION_BUTTON].buttonHoldSignal.attach(modeButtonHold);
    }

    void onFootswitchDown(ButtonEvent payload)
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
        default:
            preset->onBankItemSelect(payload.index);
            break;
        }
    }

    void onFootswitchHold(ButtonEvent event)
    {
#ifdef DEBUG
        Debug.print(DBG_VERBOSE, "Footswitch hold %d, currentMode: %d", event.index, currentMode);
#endif

        switch (currentMode)
        {
        case SwitchModes::PRESET:
            if (event.index == 0)
                preset->onBankDecrement();
            if (event.index == 1)

                preset->onBankIncrement();

            break;
        case SwitchModes::EFFECTS:
            effects->onToggleEffect(event);
            break;
        case SwitchModes::LOOPER:
            looper->handleIncomingLooperCommand(event.index);
            break;
        default:
            preset->onBankItemSelect(event.index);
            break;
        }
    }

    void onOptionPressed(ButtonEvent payload)
    {
        ToggleModes(payload);
        Debug.print(DBG_VERBOSE, "option S.ID: %d, currentMode: %d", payload.index, currentMode);
        onModeChanged();
    }

    void onOptionHold(ButtonEvent payload)
    {
        GotoLooperMode();
        Debug.print(DBG_VERBOSE, "option S.ID: %d, currentMode: %d", payload.index, static_cast<int>(currentMode));
        onModeChanged();
    }

    void ResetMode()
    {
        currentMode = SwitchModes::PRESET;
        onModeChanged();
    }

    void ToggleModes(ButtonEvent payload)
    {
        const auto nextMode = (static_cast<int>(currentMode) + 1) % static_cast<int>(SwitchModes::NUMBER_OF_MODES);
        currentMode = static_cast<SwitchModes>(nextMode);
        onModeChanged();
    }

    void GotoPresetMode()
    {
        currentMode = SwitchModes::PRESET;
        onModeChanged();
    }

    void GotoEffectMode()
    {
        currentMode = SwitchModes::EFFECTS;
        onModeChanged();
    }

    void GotoLooperMode()
    {
        currentMode = SwitchModes::LOOPER;
        onModeChanged();
    }

    void onModeChanged()
    {
        switch (currentMode)
        {
        case SwitchModes::PRESET:
            preset->onModeEnter();
            break;
        case SwitchModes::EFFECTS:
            effects->onModeEnter();
            break;
        case SwitchModes::LOOPER:
            looper->onModeEnter();
            break;
        }
    }

private:
    SwitchModes currentMode = SwitchModes::PRESET;
    PresetController *preset;
    EffectController *effects;
    LooperController *looper;
};

#endif