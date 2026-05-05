#ifndef CPU_HPP
#define CPU_HPP

#include <bitset>
#include <cstdint>
#include <memory>

enum class AddressingMode;
class Bus;

class Cpu
{
  public:
    class CpuStatus
    {
      public:
        constexpr CpuStatus()
            : status_(DEFAULT_MASK)
        {
        }
        ~CpuStatus() = default;

        constexpr void set(uint8_t value)
        {
            status_ = value;
            set_unused(1);
        }
        constexpr operator uint8_t() const { return static_cast<uint8_t>(status_.to_ulong()); }

        constexpr bool get_carry() const { return status_.test(FLAG_C_POS); }
        constexpr bool get_zero() const { return status_.test(FLAG_Z_POS); }
        constexpr bool get_interrupt() const { return status_.test(FLAG_I_POS); }
        constexpr bool get_decimal() const { return status_.test(FLAG_D_POS); }
        constexpr bool get_break() const { return status_.test(FLAG_B_POS); }
        constexpr bool get_overflow() const { return status_.test(FLAG_V_POS); }
        constexpr bool get_negative() const { return status_.test(FLAG_N_POS); }

        constexpr void set_carry(bool set) { status_.set(FLAG_C_POS, set); }
        constexpr void set_zero(bool set) { status_.set(FLAG_Z_POS, set); }
        constexpr void set_interrupt(bool set) { status_.set(FLAG_I_POS, set); }
        constexpr void set_decimal(bool set) { status_.set(FLAG_D_POS, set); }
        constexpr void set_break(bool set) { status_.set(FLAG_B_POS, set); }
        constexpr void set_unused(bool set) { status_.set(FLAG_U_POS, set); }
        constexpr void set_overflow(bool set) { status_.set(FLAG_V_POS, set); }
        constexpr void set_negative(bool set) { status_.set(FLAG_N_POS, set); }

      private:
        static constexpr uint8_t DEFAULT_MASK = 0x24;
        static constexpr uint8_t FLAG_C_POS   = 0;
        static constexpr uint8_t FLAG_Z_POS   = 1;
        static constexpr uint8_t FLAG_I_POS   = 2;
        static constexpr uint8_t FLAG_D_POS   = 3;
        static constexpr uint8_t FLAG_B_POS   = 4;
        static constexpr uint8_t FLAG_U_POS   = 5;
        static constexpr uint8_t FLAG_V_POS   = 6;
        static constexpr uint8_t FLAG_N_POS   = 7;

        std::bitset<8> status_;
    };

    Cpu() = delete;
    explicit Cpu(std::shared_ptr<Bus> bus);
    ~Cpu() = default;

    void reset();
    bool step();

    constexpr uint8_t get_register_a() const { return register_a_; }
    constexpr uint8_t get_register_x() const { return register_x_; }
    constexpr uint8_t get_register_y() const { return register_y_; }
    constexpr CpuStatus get_status() const { return status_; }
    constexpr uint16_t get_program_counter() const { return program_counter_; }
    constexpr uint8_t get_stack_pointer() const { return stack_pointer_; }

  private:
    static constexpr uint16_t STACK_BASE = 0x0100;
    static constexpr uint8_t STACK_RESET = 0xFD;

    uint8_t register_a_;
    uint8_t register_x_;
    uint8_t register_y_;
    CpuStatus status_;
    uint16_t program_counter_;
    uint8_t stack_pointer_;
    std::shared_ptr<Bus> bus_;

    constexpr void update_zero_and_negative(uint8_t value)
    {
        status_.set_zero(value == 0);
        status_.set_negative(value & 0x80);
    }

    uint16_t get_absolute_address(AddressingMode mode, uint16_t addr);
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
