#ifndef NES_CPU_HPP
#define NES_CPU_HPP

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <vector>
#include "nes_bus.hpp"
#include "nes_registers.hpp"

class NesCpu
{
  public:
    NesCpu() = delete;
    explicit NesCpu(const std::shared_ptr<NesBus>& bus);
    ~NesCpu() = default;

    void reset();
    bool step();

  private:
    static constexpr uint16_t STACK_BASE = 0x0100;
    static constexpr uint8_t STACK_RESET = 0xFD;

    enum class AddressingMode
    {
        IMMEDIATE,
        ZERO_PAGE,
        ZERO_PAGE_X,
        ZERO_PAGE_Y,
        ABSOLUTE,
        ABSOLUTE_X,
        ABSOLUTE_Y,
        INDIRECT_X,
        INDIRECT_Y,
        NONE
    };

    struct OpCode
    {
        std::string_view name;
        uint8_t length;
        uint8_t cycles;
        std::function<void(NesCpu*)> callback;
    };

    static const std::array<OpCode, 256u> OPCODES;

    uint8_t ra_;
    uint8_t rx_;
    uint8_t ry_;
    uint8_t sp_;
    uint16_t pc_;
    NesCpuStatus status_;
    std::shared_ptr<NesBus> bus_;

    void stack_push8(uint8_t value);
    void stack_push16(uint16_t value);
    uint8_t stack_pop8();
    uint16_t stack_pop16();

    uint16_t get_immediate_addr() const;
    uint16_t get_zero_page_addr() const;
    uint16_t get_zero_page_x_addr() const;
    uint16_t get_zero_page_y_addr() const;
    uint16_t get_absolute_addr() const;
    uint16_t get_absolute_x_addr() const;
    uint16_t get_absolute_y_addr() const;
    uint16_t get_indirect_x_addr() const;
    uint16_t get_indirect_y_addr() const;

    void add_to_ra(uint8_t value);

    void op_adc(uint16_t addr);
    void op_ahx(uint16_t addr);
    void op_alr(uint16_t addr);
    void op_anc(uint16_t addr);
    void op_and(uint16_t addr);
    void op_arr(uint16_t addr);
    void op_asl_accumulator();
    void op_asl(uint16_t addr);
    void op_axs(uint16_t addr);
    void op_bit(uint16_t addr);
    void op_branch(bool condition);
    void op_cmp(uint16_t addr, uint8_t compare_with);
    void op_dcp(uint16_t addr);
    void op_dec(uint16_t addr);
    void op_dex();
    void op_dey();
    void op_eor(uint16_t addr);
    void op_inc(uint16_t addr);
    void op_inx();
    void op_iny();
    void op_isb(uint16_t addr);
    void op_jmp_absolute();
    void op_jmp_indirect();
    void op_jsr();
    void op_las(uint16_t addr);
    void op_lax(uint16_t addr);
    void op_lda(uint16_t addr);
    void op_ldx(uint16_t addr);
    void op_ldy(uint16_t addr);
    void op_lsr_accumulator();
    void op_lsr(uint16_t addr);
    void op_lxa(uint16_t addr);
    void op_ora(uint16_t addr);
    void op_php();
    void op_pla();
    void op_plp();
    void op_rla(uint16_t addr);
    void op_rol_accumulator();
    void op_rol(uint16_t addr);
    void op_ror_accumulator();
    void op_ror(uint16_t addr);
    void op_rra(uint16_t addr);
    void op_rti();
    void op_rts();
    void op_sax(uint16_t addr);
    void op_sbc(uint16_t addr);
    void op_shx(uint16_t addr);
    void op_shy(uint16_t addr);
    void op_slo(uint16_t addr);
    void op_sre(uint16_t addr);
    void op_sta(uint16_t addr);
    void op_stx(uint16_t addr);
    void op_sty(uint16_t addr);
    void op_tas(uint16_t addr);
    void op_tax();
    void op_tay();
    void op_tsx();
    void op_txa();
    void op_txs();
    void op_tya();
    void op_xaa(uint16_t addr);
};

#endif /* NES_CPU_HPP */
