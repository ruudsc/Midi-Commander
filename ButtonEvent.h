#ifndef BUTTON_EVENT_H
#define BUTTON_EVENT_H
enum ButtonEventType
{
    DOWN = 0,
    UP,
    PRESSED,
    HOLD
};

struct ButtonEvent
{
    byte index;
    byte shiftRegisterIndex;
    ButtonEventType eventType;
};

#endif