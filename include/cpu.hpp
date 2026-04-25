#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include "memory.hpp"
#include "opcodes.hpp"

class Cpu
{
  public:
    std::uint8_t register_a_;
    std::uint8_t register_x_;
    std::uint8_t register_y_;
    std::uint8_t status_;
    std::uint16_t program_counter_;
    std::unique_ptr<Memory> memory_;

    Cpu()
        : register_a_(0u)
        , register_x_(0u)
        , register_y_(0u)
        , status_(0u)
        , program_counter_(0u)
        , memory_(std::make_unique<Memory>())
    {
    }

    ~Cpu() = default;

    void load_and_run(std::vector<std::uint8_t> rom);
    void load(std::vector<std::uint8_t> rom);
    void reset();
    void run();

    bool get_carry() { return get_flag<FLAG_C>(); }
    bool get_zero() { return get_flag<FLAG_Z>(); }
    bool get_interrupt() { return get_flag<FLAG_I>(); }
    bool get_decimal() { return get_flag<FLAG_D>(); }
    bool get_break() { return get_flag<FLAG_B>(); }
    bool get_overflow() { return get_flag<FLAG_V>(); }
    bool get_negative() { return get_flag<FLAG_N>(); }

  private:
    static constexpr std::uint8_t FLAG_C = 0x01;
    static constexpr std::uint8_t FLAG_Z = 0x02;
    static constexpr std::uint8_t FLAG_I = 0x04;
    static constexpr std::uint8_t FLAG_D = 0x08;
    static constexpr std::uint8_t FLAG_B = 0x10;
    static constexpr std::uint8_t FLAG_V = 0x40;
    static constexpr std::uint8_t FLAG_N = 0x80;

    template <std::uint8_t FLAG>
    void set_flag(bool set)
    {
        status_ = set ? (status_ | FLAG) : (status_ & ~FLAG);
    }

    template <std::uint8_t FLAG>
    bool get_flag()
    {
        return status_ & FLAG;
    }

    void set_carry(bool set) { set_flag<FLAG_C>(set); }
    void set_zero(bool set) { set_flag<FLAG_Z>(set); }
    void set_interrupt(bool set) { set_flag<FLAG_I>(set); }
    void set_decimal(bool set) { set_flag<FLAG_D>(set); }
    void set_break(bool set) { set_flag<FLAG_B>(set); }
    void set_overflow(bool set) { set_flag<FLAG_V>(set); }
    void set_negative(bool set) { set_flag<FLAG_N>(set); }

    void update_zero_and_negative(std::uint8_t value)
    {
        set_zero(value == 0);
        set_negative(value & 0x80);
    }

    std::uint16_t get_operand_address(AddressingMode mode);

    void op_and(AddressingMode mode);
    void op_cmp(AddressingMode mode, std::uint8_t compare_with);
    void op_inx();
    void op_lda(AddressingMode mode);
    void op_sta(AddressingMode mode);
    void op_tax();
};

#endif /* CPU_HPP */
