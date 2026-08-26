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

#endif /* NES_REGISTERS_HPP */
