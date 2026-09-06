#include "nes_joypad.hpp"

NesJoypad::NesJoypad()
    : strobe_(false)
    , button_i_(0u)
    , button_status_(0u)
{
}

void NesJoypad::reset()
{
    strobe_        = false;
    button_i_      = 0u;
    button_status_ = 0u;
}

uint8_t NesJoypad::read()
{
    if (button_i_ > 7u)
        return 1u;

    uint8_t res = (button_status_ & (1u << button_i_)) >> button_i_;
    if (!strobe_ && button_i_ <= 7u)
        button_i_ += 1u;

    return res;
}

void NesJoypad::write(uint8_t value)
{
    strobe_ = value & 1u;
    if (strobe_)
        button_i_ = 0u;
}

void NesJoypad::set_button_pressed(JoypadButton button, bool pressed)
{
    if (pressed)
        button_status_ |= static_cast<uint8_t>(button);
    else
        button_status_ &= ~static_cast<uint8_t>(button);
}
