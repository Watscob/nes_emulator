#ifndef NES_JOYPAD_HPP
#define NES_JOYPAD_HPP

#include <cstdint>

class NesJoypad
{
  public:
    enum class JoypadButton
    {
        RIGHT  = 0x80,
        LEFT   = 0x40,
        DOWN   = 0x20,
        UP     = 0x10,
        START  = 0x08,
        SELECT = 0x04,
        B      = 0x02,
        A      = 0x01
    };

    explicit NesJoypad();
    ~NesJoypad() = default;

    void reset();

    uint8_t read();
    void write(uint8_t value);

    void set_button_pressed(JoypadButton button, bool pressed);

  private:
    bool strobe_;
    uint8_t button_i_;
    uint8_t button_status_;
};

#endif /* NES_JOYPAD_HPP */
