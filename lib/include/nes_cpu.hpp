#ifndef NES_CPU_HPP
#define NES_CPU_HPP

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <tuple>
#include <vector>
#include "nes_bus.hpp"
#include "nes_registers.hpp"

class NesCpu
{
  private:
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

    static constexpr uint16_t STACK_BASE = 0x0100;
    static constexpr uint8_t STACK_RESET = 0xFD;
    static const std::array<OpCode, 256u> OPCODES;

  public:
    explicit NesCpu();
    ~NesCpu() = default;

    void connect_bus(const std::shared_ptr<NesBus>& bus);

    void reset();
    bool step();

  private:
    uint8_t ra_;
    uint8_t rx_;
    uint8_t ry_;
    uint8_t sp_;
    uint16_t pc_;
    NesCpuStatus status_;
    std::shared_ptr<NesBus> bus_;

    template <auto OpFunc, AddressingMode MODE = AddressingMode::NONE>
    static void run_op_(NesCpu* cpu);

    bool page_cross_(uint16_t addr1, uint16_t addr2) const;
    template <AddressingMode MODE>
    std::tuple<uint16_t, bool> get_operand_addr_() const;

    void stack_push8_(uint8_t value);
    void stack_push16_(uint16_t value);
    uint8_t stack_pop8_();
    uint16_t stack_pop16_();

    void add_to_ra_(uint8_t value);

    void op_adc_(uint16_t addr);
    void op_ahx_(uint16_t addr);
    void op_alr_(uint16_t addr);
    void op_anc_(uint16_t addr);
    void op_and_(uint16_t addr);
    void op_arr_(uint16_t addr);
    void op_asl_accumulator_();
    void op_asl_(uint16_t addr);
    void op_axs_(uint16_t addr);
    void op_bcc_();
    void op_bcs_();
    void op_beq_();
    void op_bit_(uint16_t addr);
    void op_bmi_();
    void op_bne_();
    void op_blp_();
    void op_branch_(bool condition);
    void op_bvc_();
    void op_bvs_();
    void op_clc_();
    void op_cld_();
    void op_cli_();
    void op_clv_();
    void op_cmp_(uint16_t addr);
    void op_cpx_(uint16_t addr);
    void op_cpy_(uint16_t addr);
    void op_dcp_(uint16_t addr);
    void op_dec_(uint16_t addr);
    void op_dex_();
    void op_dey_();
    void op_eor_(uint16_t addr);
    void op_inc_(uint16_t addr);
    void op_inx_();
    void op_iny_();
    void op_isb_(uint16_t addr);
    void op_jmp_absolute_();
    void op_jmp_indirect_();
    void op_jsr_();
    void op_las_(uint16_t addr);
    void op_lax_(uint16_t addr);
    void op_lda_(uint16_t addr);
    void op_ldx_(uint16_t addr);
    void op_ldy_(uint16_t addr);
    void op_lsr_accumulator_();
    void op_lsr_(uint16_t addr);
    void op_lxa_(uint16_t addr);
    void op_ora_(uint16_t addr);
    void op_pha_();
    void op_php_();
    void op_pla_();
    void op_plp_();
    void op_rla_(uint16_t addr);
    void op_rol_accumulator_();
    void op_rol_(uint16_t addr);
    void op_ror_accumulator_();
    void op_ror_(uint16_t addr);
    void op_rra_(uint16_t addr);
    void op_rti_();
    void op_rts_();
    void op_sax_(uint16_t addr);
    void op_sbc_(uint16_t addr);
    void op_sec_();
    void op_sed_();
    void op_sei_();
    void op_shx_(uint16_t addr);
    void op_shy_(uint16_t addr);
    void op_slo_(uint16_t addr);
    void op_sre_(uint16_t addr);
    void op_sta_(uint16_t addr);
    void op_stx_(uint16_t addr);
    void op_sty_(uint16_t addr);
    void op_tas_(uint16_t addr);
    void op_tax_();
    void op_tay_();
    void op_tsx_();
    void op_txa_();
    void op_txs_();
    void op_tya_();
    void op_xaa_(uint16_t addr);
};

#endif /* NES_CPU_HPP */
