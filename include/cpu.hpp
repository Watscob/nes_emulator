#ifndef CPU_HPP
#define CPU_HPP

#include <bitset>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include "memory.hpp"

enum class AddressingMode;

class Cpu
{
  public:
    uint8_t register_a_;
    uint8_t register_x_;
    uint8_t register_y_;
    std::bitset<8> status_;
    uint16_t program_counter_;
    uint8_t stack_pointer_;
    std::unique_ptr<Memory> memory_;

    Cpu()
        : register_a_(0u)
        , register_x_(0u)
        , register_y_(0u)
        , status_(0x24)
        , program_counter_(0x8000)
        , stack_pointer_(STACK_RESET)
        , memory_(std::make_unique<Memory>())
    {
    }

    ~Cpu() = default;

    void load_and_run(std::vector<uint8_t> rom, uint16_t start_addr = 0x8000);
    void load(std::vector<uint8_t> rom, uint16_t start_addr = 0x8000);
    void reset();
    bool execute();
    bool execute_with_callback(std::function<void(Cpu&)> callback);

    bool get_carry() { return status_.test(FLAG_C_POS); }
    bool get_zero() { return status_.test(FLAG_Z_POS); }
    bool get_interrupt() { return status_.test(FLAG_I_POS); }
    bool get_decimal() { return status_.test(FLAG_D_POS); }
    bool get_break() { return status_.test(FLAG_B_POS); }
    bool get_overflow() { return status_.test(FLAG_V_POS); }
    bool get_negative() { return status_.test(FLAG_N_POS); }

  private:
    static constexpr uint16_t STACK      = 0x0100;
    static constexpr uint8_t STACK_RESET = 0xFD;

    static constexpr uint8_t FLAG_C_POS = 0;
    static constexpr uint8_t FLAG_Z_POS = 1;
    static constexpr uint8_t FLAG_I_POS = 2;
    static constexpr uint8_t FLAG_D_POS = 3;
    static constexpr uint8_t FLAG_B_POS = 4;
    static constexpr uint8_t FLAG_V_POS = 6;
    static constexpr uint8_t FLAG_N_POS = 7;

    void set_carry(bool set) { status_.set(FLAG_C_POS, set); }
    void set_zero(bool set) { status_.set(FLAG_Z_POS, set); }
    void set_interrupt(bool set) { status_.set(FLAG_I_POS, set); }
    void set_decimal(bool set) { status_.set(FLAG_D_POS, set); }
    void set_break(bool set) { status_.set(FLAG_B_POS, set); }
    void set_overflow(bool set) { status_.set(FLAG_V_POS, set); }
    void set_negative(bool set) { status_.set(FLAG_N_POS, set); }

    void update_zero_and_negative(uint8_t value)
    {
        set_zero(value == 0);
        set_negative(value & 0x80);
    }

    uint16_t get_operand_address(AddressingMode mode);

    void stack_push8(uint8_t data);
    uint8_t stack_pop8();
    void stack_push16(uint16_t data);
    uint16_t stack_pop16();

    void add_to_register_a(uint8_t value);

    void op_adc(AddressingMode mode);
    void op_and(AddressingMode mode);
    void op_asl_accumulator();
    void op_asl(AddressingMode mode);
    void op_branch(bool condition);
    void op_bit(AddressingMode mode);
    void op_cmp(AddressingMode mode, uint8_t compare_with);
    void op_dec(AddressingMode mode);
    void op_dex();
    void op_dey();
    void op_eor(AddressingMode mode);
    void op_inc(AddressingMode mode);
    void op_inx();
    void op_iny();
    void op_jmp_absolute();
    void op_jmp_indirect();
    void op_jsr();
    void op_lda(AddressingMode mode);
    void op_ldx(AddressingMode mode);
    void op_ldy(AddressingMode mode);
    void op_lsr_accumulator();
    void op_lsr(AddressingMode mode);
    void op_ora(AddressingMode mode);
    void op_php();
    void op_pla();
    void op_plp();
    void op_rol_accumulator();
    void op_rol(AddressingMode mode);
    void op_ror_accumulator();
    void op_ror(AddressingMode mode);
    void op_rti();
    void op_rts();
    void op_sbc(AddressingMode mode);
    void op_sta(AddressingMode mode);
    void op_stx(AddressingMode mode);
    void op_sty(AddressingMode mode);
    void op_tax();
    void op_tay();
    void op_tsx();
    void op_txa();
    void op_txs();
    void op_tya();
};

#endif /* CPU_HPP */
