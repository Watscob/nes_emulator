#ifndef NES_REGISTERS_HPP
#define NES_REGISTERS_HPP

#include <concepts>
#include <cstdint>

template <std::integral T, T DEFAULT>
class NesRegister
{
  public:
    constexpr explicit NesRegister()
        : reg_(DEFAULT)
    {
    }

    constexpr void set(const T value) { reg_ = value; }

    constexpr T get() const { return reg_; }

  protected:
    template <T MASK>
    constexpr bool get_bit_() const
    {
        return reg_ & MASK;
    }

    template <T MASK, T SHIFT>
    constexpr T get_field_() const
    {
        return static_cast<T>((reg_ & MASK) >> SHIFT);
    }

    template <T MASK>
    constexpr void set_bit_(bool set)
    {
        if (set)
            reg_ |= MASK;
        else
            reg_ &= ~MASK;
    }

    template <T MASK, T SHIFT>
    constexpr void set_field_(T val)
    {
        reg_ = static_cast<T>((reg_ & static_cast<T>(~MASK)) | ((val << SHIFT) & MASK));
    }

  private:
    T reg_;
};

class NesCpuStatus : public NesRegister<uint8_t, 0x24>
{
  private:
    static constexpr uint8_t C_MASK = 0x01;
    static constexpr uint8_t Z_MASK = 0x02;
    static constexpr uint8_t I_MASK = 0x04;
    static constexpr uint8_t D_MASK = 0x08;
    static constexpr uint8_t B_MASK = 0x10;
    static constexpr uint8_t U_MASK = 0x20;
    static constexpr uint8_t V_MASK = 0x40;
    static constexpr uint8_t N_MASK = 0x80;

  public:
    constexpr bool c() const { return get_bit_<C_MASK>(); }
    constexpr bool z() const { return get_bit_<Z_MASK>(); }
    constexpr bool i() const { return get_bit_<I_MASK>(); }
    constexpr bool d() const { return get_bit_<D_MASK>(); }
    constexpr bool b() const { return get_bit_<B_MASK>(); }
    constexpr bool v() const { return get_bit_<V_MASK>(); }
    constexpr bool n() const { return get_bit_<N_MASK>(); }

    constexpr void c(bool set) { set_bit_<C_MASK>(set); }
    constexpr void z(bool set) { set_bit_<Z_MASK>(set); }
    constexpr void i(bool set) { set_bit_<I_MASK>(set); }
    constexpr void d(bool set) { set_bit_<D_MASK>(set); }
    constexpr void b(bool set) { set_bit_<B_MASK>(set); }
    constexpr void u(bool set) { set_bit_<U_MASK>(set); }
    constexpr void v(bool set) { set_bit_<V_MASK>(set); }
    constexpr void n(bool set) { set_bit_<N_MASK>(set); }

    constexpr void set_nz(uint8_t value)
    {
        z(value == 0u);
        n(value & N_MASK);
    }
};

class NesPpuCtrl : public NesRegister<uint8_t, 0x00>
{
  private:
    static constexpr uint8_t NAMETABLE_MASK     = 0x03;
    static constexpr uint8_t NAMETABLE_SHIFT    = 0x00;
    static constexpr uint8_t INCREMENT_MASK     = 0x04;
    static constexpr uint8_t SPRITE_TABLE_MASK  = 0x08;
    static constexpr uint8_t BACKGROUND_MASK    = 0x10;
    static constexpr uint8_t SPRITE_HEIGHT_MASK = 0x20;
    static constexpr uint8_t MASTER_MODE_MASK   = 0x40;
    static constexpr uint8_t NMI_ENABLE_MASK    = 0x80;

  public:
    constexpr uint8_t nametable() const { return get_field_<NAMETABLE_MASK, NAMETABLE_SHIFT>(); }
    constexpr bool increment() const { return get_bit_<INCREMENT_MASK>(); }
    constexpr bool sprite_table() const { return get_bit_<SPRITE_TABLE_MASK>(); }
    constexpr bool background() const { return get_bit_<BACKGROUND_MASK>(); }
    constexpr bool sprite_height() const { return get_bit_<SPRITE_HEIGHT_MASK>(); }
    constexpr bool master_mode() const { return get_bit_<MASTER_MODE_MASK>(); }
    constexpr bool nmi_enable() const { return get_bit_<NMI_ENABLE_MASK>(); }

    constexpr void nametable(uint8_t val) { set_field_<NAMETABLE_MASK, NAMETABLE_SHIFT>(val); }
    constexpr void increment(bool set) { set_bit_<INCREMENT_MASK>(set); }
    constexpr void sprite_table(bool set) { set_bit_<SPRITE_TABLE_MASK>(set); }
    constexpr void background(bool set) { set_bit_<BACKGROUND_MASK>(set); }
    constexpr void sprite_height(bool set) { set_bit_<SPRITE_HEIGHT_MASK>(set); }
    constexpr void master_mode(bool set) { set_bit_<MASTER_MODE_MASK>(set); }
    constexpr void nmi_enable(bool set) { set_bit_<NMI_ENABLE_MASK>(set); }
};

class NesPpuMask : public NesRegister<uint8_t, 0x00>
{
  private:
    static constexpr uint8_t GREYSCALE_MASK          = 0x01;
    static constexpr uint8_t SHOW_BG_LEFT8_MASK      = 0x02;
    static constexpr uint8_t SHOW_SPRITES_LEFT8_MASK = 0x04;
    static constexpr uint8_t SHOW_BG_MASK            = 0x08;
    static constexpr uint8_t SHOW_SPRITES_MASK       = 0x10;
    static constexpr uint8_t EMPHASIZE_RED_MASK      = 0x20;
    static constexpr uint8_t EMPHASIZE_GREEN_MASK    = 0x40;
    static constexpr uint8_t EMPHASIZE_BLUE_MASK     = 0x80;

  public:
    constexpr bool greyscale() const { return get_bit_<GREYSCALE_MASK>(); }
    constexpr bool show_bg_left8() const { return get_bit_<SHOW_BG_LEFT8_MASK>(); }
    constexpr bool show_sprites_left8() const { return get_bit_<SHOW_SPRITES_LEFT8_MASK>(); }
    constexpr bool show_bg() const { return get_bit_<SHOW_BG_MASK>(); }
    constexpr bool show_sprites() const { return get_bit_<SHOW_SPRITES_MASK>(); }
    constexpr bool emphasize_red() const { return get_bit_<EMPHASIZE_RED_MASK>(); }
    constexpr bool emphasize_green() const { return get_bit_<EMPHASIZE_GREEN_MASK>(); }
    constexpr bool emphasize_blue() const { return get_bit_<EMPHASIZE_BLUE_MASK>(); }

    constexpr void greyscale(bool set) { set_bit_<GREYSCALE_MASK>(set); }
    constexpr void show_bg_left8(bool set) { set_bit_<SHOW_BG_LEFT8_MASK>(set); }
    constexpr void show_sprites_left8(bool set) { set_bit_<SHOW_SPRITES_LEFT8_MASK>(set); }
    constexpr void show_bg(bool set) { set_bit_<SHOW_BG_MASK>(set); }
    constexpr void show_sprites(bool set) { set_bit_<SHOW_SPRITES_MASK>(set); }
    constexpr void emphasize_red(bool set) { set_bit_<EMPHASIZE_RED_MASK>(set); }
    constexpr void emphasize_green(bool set) { set_bit_<EMPHASIZE_GREEN_MASK>(set); }
    constexpr void emphasize_blue(bool set) { set_bit_<EMPHASIZE_BLUE_MASK>(set); }
};

class NesPpuStatus : public NesRegister<uint8_t, 0x00>
{
  private:
    static constexpr uint8_t SPRITE_OVERFLOW_MASK = 0x20;
    static constexpr uint8_t SPRITE_ZERO_HIT_MASK = 0x40;
    static constexpr uint8_t VBLANK_MASK          = 0x80;

  public:
    constexpr bool sprite_overflow() const { return get_bit_<SPRITE_OVERFLOW_MASK>(); }
    constexpr bool sprite_zero_hit() const { return get_bit_<SPRITE_ZERO_HIT_MASK>(); }
    constexpr bool vblank() const { return get_bit_<VBLANK_MASK>(); }

    constexpr void sprite_overflow(bool set) { set_bit_<SPRITE_OVERFLOW_MASK>(set); }
    constexpr void sprite_zero_hit(bool set) { set_bit_<SPRITE_ZERO_HIT_MASK>(set); }
    constexpr void vblank(bool set) { set_bit_<VBLANK_MASK>(set); }
};

#endif /* NES_REGISTERS_HPP */
