#include "nes_cpu.hpp"

const std::array<NesCpu::OpCode, 256u> NesCpu::OPCODES = {{
    /* 0x00 */ {"BRK", 1, 7, [](NesCpu*) {}},
    /* 0x01 */ {"ORA", 2, 6, [](NesCpu* cpu) { cpu->op_ora(cpu->get_indirect_x_addr()); }},
    /* 0x02 */ {"*NOP", 1, 2, nullptr},
    /* 0x03 */ {"*SLO", 2, 8, [](NesCpu* cpu) { cpu->op_slo(cpu->get_indirect_x_addr()); }},
    /* 0x04 */ {"*NOP", 2, 3, nullptr},
    /* 0x05 */ {"ORA", 2, 3, [](NesCpu* cpu) { cpu->op_ora(cpu->get_zero_page_addr()); }},
    /* 0x06 */ {"ASL", 2, 5, [](NesCpu* cpu) { cpu->op_asl(cpu->get_zero_page_addr()); }},
    /* 0x07 */ {"*SLO", 2, 5, [](NesCpu* cpu) { cpu->op_slo(cpu->get_zero_page_addr()); }},
    /* 0x08 */ {"PHP", 1, 3, [](NesCpu* cpu) { cpu->op_php(); }},
    /* 0x09 */ {"ORA", 2, 2, [](NesCpu* cpu) { cpu->op_ora(cpu->get_immediate_addr()); }},
    /* 0x0A */ {"ASL", 1, 2, [](NesCpu* cpu) { cpu->op_asl_accumulator(); }},
    /* 0x0B */ {"*ANC", 2, 2, [](NesCpu* cpu) { cpu->op_anc(cpu->get_immediate_addr()); }},
    /* 0x0C */ {"*NOP", 3, 4, nullptr},
    /* 0x0D */ {"ORA", 3, 4, [](NesCpu* cpu) { cpu->op_ora(cpu->get_absolute_addr()); }},
    /* 0x0E */ {"ASL", 3, 6, [](NesCpu* cpu) { cpu->op_asl(cpu->get_absolute_addr()); }},
    /* 0x0F */ {"*SLO", 3, 6, [](NesCpu* cpu) { cpu->op_slo(cpu->get_absolute_addr()); }},
    /* 0x10 */
    {"BPL",
     2,
     2 /*+1 if branch succeeds, +2 if to a new page*/,
     [](NesCpu* cpu) { cpu->op_branch(!cpu->status_.n()); }},
    /* 0x11 */
    {"ORA",
     2,
     5 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_ora(cpu->get_indirect_y_addr()); }},
    /* 0x12 */ {"*NOP", 1, 2, nullptr},
    /* 0x13 */ {"*SLO", 2, 8, [](NesCpu* cpu) { cpu->op_slo(cpu->get_indirect_y_addr()); }},
    /* 0x14 */ {"*NOP", 2, 4, nullptr},
    /* 0x15 */ {"ORA", 2, 4, [](NesCpu* cpu) { cpu->op_ora(cpu->get_zero_page_x_addr()); }},
    /* 0x16 */ {"ASL", 2, 6, [](NesCpu* cpu) { cpu->op_asl(cpu->get_zero_page_x_addr()); }},
    /* 0x17 */ {"*SLO", 2, 6, [](NesCpu* cpu) { cpu->op_slo(cpu->get_zero_page_x_addr()); }},
    /* 0x18 */
    {"CLC", 1, 2, [](NesCpu* cpu) { cpu->status_.c(0u); }},
    /* 0x19 */
    {"ORA",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_ora(cpu->get_absolute_y_addr()); }},
    /* 0x1A */ {"*NOP", 1, 2, nullptr},
    /* 0x1B */ {"*SLO", 3, 7, [](NesCpu* cpu) { cpu->op_slo(cpu->get_absolute_y_addr()); }},
    /* 0x1C */ {"*NOP", 3, 4 /*+1 if page crossed*/, nullptr},
    /* 0x1D */
    {"ORA",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_ora(cpu->get_absolute_x_addr()); }},
    /* 0x1E */ {"ASL", 3, 7, [](NesCpu* cpu) { cpu->op_asl(cpu->get_absolute_x_addr()); }},
    /* 0x1F */ {"*SLO", 3, 7, [](NesCpu* cpu) { cpu->op_slo(cpu->get_absolute_x_addr()); }},
    /* 0x20 */ {"JSR", 3, 6, [](NesCpu* cpu) { cpu->op_jsr(); }},
    /* 0x21 */ {"AND", 2, 6, [](NesCpu* cpu) { cpu->op_and(cpu->get_indirect_x_addr()); }},
    /* 0x22 */ {"*NOP", 1, 2, nullptr},
    /* 0x23 */ {"*RLA", 2, 8, [](NesCpu* cpu) { cpu->op_rla(cpu->get_indirect_x_addr()); }},
    /* 0x24 */ {"BIT", 2, 3, [](NesCpu* cpu) { cpu->op_bit(cpu->get_zero_page_addr()); }},
    /* 0x25 */ {"AND", 2, 3, [](NesCpu* cpu) { cpu->op_and(cpu->get_zero_page_addr()); }},
    /* 0x26 */ {"ROL", 2, 5, [](NesCpu* cpu) { cpu->op_rol(cpu->get_zero_page_addr()); }},
    /* 0x27 */ {"*RLA", 2, 5, [](NesCpu* cpu) { cpu->op_rla(cpu->get_zero_page_addr()); }},
    /* 0x28 */ {"PLP", 1, 4, [](NesCpu* cpu) { cpu->op_plp(); }},
    /* 0x29 */ {"AND", 2, 2, [](NesCpu* cpu) { cpu->op_and(cpu->get_immediate_addr()); }},
    /* 0x2A */ {"ROL", 1, 2, [](NesCpu* cpu) { cpu->op_rol_accumulator(); }},
    /* 0x2B */ {"*ANC", 2, 2, [](NesCpu* cpu) { cpu->op_anc(cpu->get_immediate_addr()); }},
    /* 0x2C */ {"BIT", 3, 4, [](NesCpu* cpu) { cpu->op_bit(cpu->get_absolute_addr()); }},
    /* 0x2D */ {"AND", 3, 4, [](NesCpu* cpu) { cpu->op_and(cpu->get_absolute_addr()); }},
    /* 0x2E */ {"ROL", 3, 6, [](NesCpu* cpu) { cpu->op_rol(cpu->get_absolute_addr()); }},
    /* 0x2F */ {"*RLA", 3, 6, [](NesCpu* cpu) { cpu->op_rla(cpu->get_absolute_addr()); }},
    /* 0x30 */
    {"BMI",
     2,
     2 /*+1 if branch succeeds, +2 if to a new page*/,
     [](NesCpu* cpu) { cpu->op_branch(cpu->status_.n()); }},
    /* 0x31 */
    {"AND",
     2,
     5 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_and(cpu->get_indirect_y_addr()); }},
    /* 0x32 */ {"*NOP", 1, 2, nullptr},
    /* 0x33 */ {"*RLA", 2, 8, [](NesCpu* cpu) { cpu->op_rla(cpu->get_indirect_y_addr()); }},
    /* 0x34 */ {"*NOP", 2, 4, nullptr},
    /* 0x35 */ {"AND", 2, 4, [](NesCpu* cpu) { cpu->op_and(cpu->get_zero_page_x_addr()); }},
    /* 0x36 */ {"ROL", 2, 6, [](NesCpu* cpu) { cpu->op_rol(cpu->get_zero_page_x_addr()); }},
    /* 0x37 */ {"*RLA", 2, 6, [](NesCpu* cpu) { cpu->op_rla(cpu->get_zero_page_x_addr()); }},
    /* 0x38 */
    {"SEC", 1, 2, [](NesCpu* cpu) { cpu->status_.c(1u); }},
    /* 0x39 */
    {"AND",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_and(cpu->get_absolute_y_addr()); }},
    /* 0x3A */ {"*NOP", 1, 2, nullptr},
    /* 0x3B */ {"*RLA", 3, 7, [](NesCpu* cpu) { cpu->op_rla(cpu->get_absolute_y_addr()); }},
    /* 0x3C */ {"*NOP", 3, 4 /*+1 if page crossed*/, nullptr},
    /* 0x3D */
    {"AND",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_and(cpu->get_absolute_x_addr()); }},
    /* 0x3E */ {"ROL", 3, 7, [](NesCpu* cpu) { cpu->op_rol(cpu->get_absolute_x_addr()); }},
    /* 0x3F */ {"*RLA", 3, 7, [](NesCpu* cpu) { cpu->op_rla(cpu->get_absolute_x_addr()); }},
    /* 0x40 */ {"RTI", 1, 6, [](NesCpu* cpu) { cpu->op_rti(); }},
    /* 0x41 */ {"EOR", 2, 6, [](NesCpu* cpu) { cpu->op_eor(cpu->get_indirect_x_addr()); }},
    /* 0x42 */ {"*NOP", 1, 2, nullptr},
    /* 0x43 */ {"*SRE", 2, 8, [](NesCpu* cpu) { cpu->op_sre(cpu->get_indirect_x_addr()); }},
    /* 0x44 */ {"*NOP", 2, 3, nullptr},
    /* 0x45 */ {"EOR", 2, 3, [](NesCpu* cpu) { cpu->op_eor(cpu->get_zero_page_addr()); }},
    /* 0x46 */ {"LSR", 2, 5, [](NesCpu* cpu) { cpu->op_lsr(cpu->get_zero_page_addr()); }},
    /* 0x47 */ {"*SRE", 2, 5, [](NesCpu* cpu) { cpu->op_sre(cpu->get_zero_page_addr()); }},
    /* 0x48 */
    {"PHA", 1, 3, [](NesCpu* cpu) { cpu->stack_push8(cpu->ra_); }},
    /* 0x49 */ {"EOR", 2, 2, [](NesCpu* cpu) { cpu->op_eor(cpu->get_immediate_addr()); }},
    /* 0x4A */ {"LSR", 1, 2, [](NesCpu* cpu) { cpu->op_lsr_accumulator(); }},
    /* 0x4B */ {"*ALR", 2, 2, [](NesCpu* cpu) { cpu->op_alr(cpu->get_immediate_addr()); }},
    /* 0x4C */ {"JMP", 3, 3, [](NesCpu* cpu) { cpu->op_jmp_absolute(); }},
    /* 0x4D */ {"EOR", 3, 4, [](NesCpu* cpu) { cpu->op_eor(cpu->get_absolute_addr()); }},
    /* 0x4E */ {"LSR", 3, 6, [](NesCpu* cpu) { cpu->op_lsr(cpu->get_absolute_addr()); }},
    /* 0x4F */ {"*SRE", 3, 6, [](NesCpu* cpu) { cpu->op_sre(cpu->get_absolute_addr()); }},
    /* 0x50 */
    {"BVC",
     2,
     2 /*+1 if branch succeeds, +2 if to a new page*/,
     [](NesCpu* cpu) { cpu->op_branch(!cpu->status_.v()); }},
    /* 0x51 */
    {"EOR",
     2,
     5 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_eor(cpu->get_indirect_y_addr()); }},
    /* 0x52 */ {"*NOP", 1, 2, nullptr},
    /* 0x53 */ {"*SRE", 2, 8, [](NesCpu* cpu) { cpu->op_sre(cpu->get_indirect_y_addr()); }},
    /* 0x54 */ {"*NOP", 2, 4, nullptr},
    /* 0x55 */ {"EOR", 2, 4, [](NesCpu* cpu) { cpu->op_eor(cpu->get_zero_page_x_addr()); }},
    /* 0x56 */ {"LSR", 2, 6, [](NesCpu* cpu) { cpu->op_lsr(cpu->get_zero_page_x_addr()); }},
    /* 0x57 */ {"*SRE", 2, 6, [](NesCpu* cpu) { cpu->op_sre(cpu->get_zero_page_x_addr()); }},
    /* 0x58 */
    {"CLI", 1, 2, [](NesCpu* cpu) { cpu->status_.i(0u); }},
    /* 0x59 */
    {"EOR",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_eor(cpu->get_absolute_y_addr()); }},
    /* 0x5A */ {"*NOP", 1, 2, nullptr},
    /* 0x5B */ {"*SRE", 3, 7, [](NesCpu* cpu) { cpu->op_sre(cpu->get_absolute_y_addr()); }},
    /* 0x5C */ {"*NOP", 3, 4 /*+1 if page crossed*/, nullptr},
    /* 0x5D */
    {"EOR",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_eor(cpu->get_absolute_x_addr()); }},
    /* 0x5E */ {"LSR", 3, 7, [](NesCpu* cpu) { cpu->op_lsr(cpu->get_absolute_x_addr()); }},
    /* 0x5F */ {"*SRE", 3, 7, [](NesCpu* cpu) { cpu->op_sre(cpu->get_absolute_x_addr()); }},
    /* 0x60 */ {"RTS", 1, 6, [](NesCpu* cpu) { cpu->op_rts(); }},
    /* 0x61 */ {"ADC", 2, 6, [](NesCpu* cpu) { cpu->op_adc(cpu->get_indirect_x_addr()); }},
    /* 0x62 */ {"*NOP", 1, 2, nullptr},
    /* 0x63 */ {"*RRA", 2, 8, [](NesCpu* cpu) { cpu->op_rra(cpu->get_indirect_x_addr()); }},
    /* 0x64 */ {"*NOP", 2, 3, nullptr},
    /* 0x65 */ {"ADC", 2, 3, [](NesCpu* cpu) { cpu->op_adc(cpu->get_zero_page_addr()); }},
    /* 0x66 */ {"ROR", 2, 5, [](NesCpu* cpu) { cpu->op_ror(cpu->get_zero_page_addr()); }},
    /* 0x67 */ {"*RRA", 2, 5, [](NesCpu* cpu) { cpu->op_rra(cpu->get_zero_page_addr()); }},
    /* 0x68 */ {"PLA", 1, 4, [](NesCpu* cpu) { cpu->op_pla(); }},
    /* 0x69 */ {"ADC", 2, 2, [](NesCpu* cpu) { cpu->op_adc(cpu->get_immediate_addr()); }},
    /* 0x6A */ {"ROR", 1, 2, [](NesCpu* cpu) { cpu->op_ror_accumulator(); }},
    /* 0x6B */ {"*ARR", 2, 2, [](NesCpu* cpu) { cpu->op_arr(cpu->get_immediate_addr()); }},
    /* 0x6C */ {"JMP", 3, 5, [](NesCpu* cpu) { cpu->op_jmp_indirect(); }},
    /* 0x6D */ {"ADC", 3, 4, [](NesCpu* cpu) { cpu->op_adc(cpu->get_absolute_addr()); }},
    /* 0x6E */ {"ROR", 3, 6, [](NesCpu* cpu) { cpu->op_ror(cpu->get_absolute_addr()); }},
    /* 0x6F */ {"*RRA", 3, 6, [](NesCpu* cpu) { cpu->op_rra(cpu->get_absolute_addr()); }},
    /* 0x70 */
    {"BVS",
     2,
     2 /*+1 if branch succeeds, +2 if to a new page*/,
     [](NesCpu* cpu) { cpu->op_branch(cpu->status_.v()); }},
    /* 0x71 */
    {"ADC",
     2,
     5 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_adc(cpu->get_indirect_y_addr()); }},
    /* 0x72 */ {"*NOP", 1, 2, nullptr},
    /* 0x73 */ {"*RRA", 2, 8, [](NesCpu* cpu) { cpu->op_rra(cpu->get_indirect_y_addr()); }},
    /* 0x74 */ {"*NOP", 2, 4, nullptr},
    /* 0x75 */ {"ADC", 2, 4, [](NesCpu* cpu) { cpu->op_adc(cpu->get_zero_page_x_addr()); }},
    /* 0x76 */ {"ROR", 2, 6, [](NesCpu* cpu) { cpu->op_ror(cpu->get_zero_page_x_addr()); }},
    /* 0x77 */ {"*RRA", 2, 6, [](NesCpu* cpu) { cpu->op_rra(cpu->get_zero_page_x_addr()); }},
    /* 0x78 */
    {"SEI", 1, 2, [](NesCpu* cpu) { cpu->status_.i(1u); }},
    /* 0x79 */
    {"ADC",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_adc(cpu->get_absolute_y_addr()); }},
    /* 0x7A */ {"*NOP", 1, 2, nullptr},
    /* 0x7B */ {"*RRA", 3, 7, [](NesCpu* cpu) { cpu->op_rra(cpu->get_absolute_y_addr()); }},
    /* 0x7C */ {"*NOP", 3, 4 /*+1 if page crossed*/, nullptr},
    /* 0x7D */
    {"ADC",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_adc(cpu->get_absolute_x_addr()); }},
    /* 0x7E */ {"ROR", 3, 7, [](NesCpu* cpu) { cpu->op_ror(cpu->get_absolute_x_addr()); }},
    /* 0x7F */ {"*RRA", 3, 7, [](NesCpu* cpu) { cpu->op_rra(cpu->get_absolute_x_addr()); }},
    /* 0x80 */ {"*NOP", 2, 2, nullptr},
    /* 0x81 */ {"STA", 2, 6, [](NesCpu* cpu) { cpu->op_sta(cpu->get_indirect_x_addr()); }},
    /* 0x82 */ {"*NOP", 2, 2, nullptr},
    /* 0x83 */ {"*SAX", 2, 6, [](NesCpu* cpu) { cpu->op_sax(cpu->get_indirect_x_addr()); }},
    /* 0x84 */ {"STY", 2, 3, [](NesCpu* cpu) { cpu->op_sty(cpu->get_zero_page_addr()); }},
    /* 0x85 */ {"STA", 2, 3, [](NesCpu* cpu) { cpu->op_sta(cpu->get_zero_page_addr()); }},
    /* 0x86 */ {"STX", 2, 3, [](NesCpu* cpu) { cpu->op_stx(cpu->get_zero_page_addr()); }},
    /* 0x87 */ {"*SAX", 2, 3, [](NesCpu* cpu) { cpu->op_sax(cpu->get_zero_page_addr()); }},
    /* 0x88 */ {"DEY", 1, 2, [](NesCpu* cpu) { cpu->op_dey(); }},
    /* 0x89 */ {"*NOP", 2, 2, nullptr},
    /* 0x8A */ {"TXA", 1, 2, [](NesCpu* cpu) { cpu->op_txa(); }},
    /* 0x8B */ {"*XAA", 2, 2, [](NesCpu* cpu) { cpu->op_xaa(cpu->get_immediate_addr()); }},
    /* 0x8C */ {"STY", 3, 4, [](NesCpu* cpu) { cpu->op_sty(cpu->get_absolute_addr()); }},
    /* 0x8D */ {"STA", 3, 4, [](NesCpu* cpu) { cpu->op_sta(cpu->get_absolute_addr()); }},
    /* 0x8E */ {"STX", 3, 4, [](NesCpu* cpu) { cpu->op_stx(cpu->get_absolute_addr()); }},
    /* 0x8F */ {"*SAX", 3, 4, [](NesCpu* cpu) { cpu->op_sax(cpu->get_absolute_addr()); }},
    /* 0x90 */
    {"BCC",
     2,
     2 /*+1 if branch succeeds, +2 if to a new page*/,
     [](NesCpu* cpu) { cpu->op_branch(!cpu->status_.c()); }},
    /* 0x91 */ {"STA", 2, 6, [](NesCpu* cpu) { cpu->op_sta(cpu->get_indirect_y_addr()); }},
    /* 0x92 */ {"*NOP", 1, 2, nullptr},
    /* 0x93 */ {"*AHX", 2, 6, [](NesCpu* cpu) { cpu->op_ahx(cpu->get_indirect_y_addr()); }},
    /* 0x94 */ {"STY", 2, 4, [](NesCpu* cpu) { cpu->op_sty(cpu->get_zero_page_x_addr()); }},
    /* 0x95 */ {"STA", 2, 4, [](NesCpu* cpu) { cpu->op_sta(cpu->get_zero_page_x_addr()); }},
    /* 0x96 */ {"STX", 2, 4, [](NesCpu* cpu) { cpu->op_stx(cpu->get_zero_page_y_addr()); }},
    /* 0x97 */ {"*SAX", 2, 4, [](NesCpu* cpu) { cpu->op_sax(cpu->get_zero_page_y_addr()); }},
    /* 0x98 */ {"TYA", 1, 2, [](NesCpu* cpu) { cpu->op_tya(); }},
    /* 0x99 */ {"STA", 3, 5, [](NesCpu* cpu) { cpu->op_sta(cpu->get_absolute_y_addr()); }},
    /* 0x9A */ {"TXS", 1, 2, [](NesCpu* cpu) { cpu->op_txs(); }},
    /* 0x9B */ {"*TAS", 3, 5, [](NesCpu* cpu) { cpu->op_tas(cpu->get_absolute_y_addr()); }},
    /* 0x9C */ {"*SHY", 3, 5, [](NesCpu* cpu) { cpu->op_shy(cpu->get_absolute_x_addr()); }},
    /* 0x9D */ {"STA", 3, 5, [](NesCpu* cpu) { cpu->op_sta(cpu->get_absolute_x_addr()); }},
    /* 0x9E */ {"*SHX", 3, 5, [](NesCpu* cpu) { cpu->op_shx(cpu->get_absolute_y_addr()); }},
    /* 0x9F */ {"*AHX", 3, 5, [](NesCpu* cpu) { cpu->op_ahx(cpu->get_absolute_y_addr()); }},
    /* 0xA0 */ {"LDY", 2, 2, [](NesCpu* cpu) { cpu->op_ldy(cpu->get_immediate_addr()); }},
    /* 0xA1 */ {"LDA", 2, 6, [](NesCpu* cpu) { cpu->op_lda(cpu->get_indirect_x_addr()); }},
    /* 0xA2 */ {"LDX", 2, 2, [](NesCpu* cpu) { cpu->op_ldx(cpu->get_immediate_addr()); }},
    /* 0xA3 */ {"*LAX", 2, 6, [](NesCpu* cpu) { cpu->op_lax(cpu->get_indirect_x_addr()); }},
    /* 0xA4 */ {"LDY", 2, 3, [](NesCpu* cpu) { cpu->op_ldy(cpu->get_zero_page_addr()); }},
    /* 0xA5 */ {"LDA", 2, 3, [](NesCpu* cpu) { cpu->op_lda(cpu->get_zero_page_addr()); }},
    /* 0xA6 */ {"LDX", 2, 3, [](NesCpu* cpu) { cpu->op_ldx(cpu->get_zero_page_addr()); }},
    /* 0xA7 */ {"*LAX", 2, 3, [](NesCpu* cpu) { cpu->op_lax(cpu->get_zero_page_addr()); }},
    /* 0xA8 */ {"TAY", 1, 2, [](NesCpu* cpu) { cpu->op_tay(); }},
    /* 0xA9 */ {"LDA", 2, 2, [](NesCpu* cpu) { cpu->op_lda(cpu->get_immediate_addr()); }},
    /* 0xAA */ {"TAX", 1, 2, [](NesCpu* cpu) { cpu->op_tax(); }},
    /* 0xAB */ {"*LXA", 2, 2, [](NesCpu* cpu) { cpu->op_lxa(cpu->get_immediate_addr()); }},
    /* 0xAC */ {"LDY", 3, 4, [](NesCpu* cpu) { cpu->op_ldy(cpu->get_absolute_addr()); }},
    /* 0xAD */ {"LDA", 3, 4, [](NesCpu* cpu) { cpu->op_lda(cpu->get_absolute_addr()); }},
    /* 0xAE */ {"LDX", 3, 4, [](NesCpu* cpu) { cpu->op_ldx(cpu->get_absolute_addr()); }},
    /* 0xAF */ {"*LAX", 3, 4, [](NesCpu* cpu) { cpu->op_lax(cpu->get_absolute_addr()); }},
    /* 0xB0 */
    {"BCS",
     2,
     2 /*+1 if branch succeeds, +2 if to a new page*/,
     [](NesCpu* cpu) { cpu->op_branch(cpu->status_.c()); }},
    /* 0xB1 */
    {"LDA",
     2,
     5 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_lda(cpu->get_indirect_y_addr()); }},
    /* 0xB2 */ {"*NOP", 1, 2, nullptr},
    /* 0xB3 */
    {"*LAX",
     2,
     5 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_lax(cpu->get_indirect_y_addr()); }},
    /* 0xB4 */ {"LDY", 2, 4, [](NesCpu* cpu) { cpu->op_ldy(cpu->get_zero_page_x_addr()); }},
    /* 0xB5 */ {"LDA", 2, 4, [](NesCpu* cpu) { cpu->op_lda(cpu->get_zero_page_x_addr()); }},
    /* 0xB6 */ {"LDX", 2, 4, [](NesCpu* cpu) { cpu->op_ldx(cpu->get_zero_page_y_addr()); }},
    /* 0xB7 */ {"*LAX", 2, 4, [](NesCpu* cpu) { cpu->op_lax(cpu->get_zero_page_y_addr()); }},
    /* 0xB8 */
    {"CLV", 1, 2, [](NesCpu* cpu) { cpu->status_.v(0u); }},
    /* 0xB9 */
    {"LDA",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_lda(cpu->get_absolute_y_addr()); }},
    /* 0xBA */ {"TSX", 1, 2, [](NesCpu* cpu) { cpu->op_tsx(); }},
    /* 0xBB */
    {"*LAS",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_las(cpu->get_absolute_y_addr()); }},
    /* 0xBC */
    {"LDY",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_ldy(cpu->get_absolute_x_addr()); }},
    /* 0xBD */
    {"LDA",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_lda(cpu->get_absolute_x_addr()); }},
    /* 0xBE */
    {"LDX",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_ldx(cpu->get_absolute_y_addr()); }},
    /* 0xBF */
    {"*LAX",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_lax(cpu->get_absolute_y_addr()); }},
    /* 0xC0 */
    {"CPY", 2, 2, [](NesCpu* cpu) { cpu->op_cmp(cpu->get_immediate_addr(), cpu->ry_); }},
    /* 0xC1 */
    {"CMP", 2, 6, [](NesCpu* cpu) { cpu->op_cmp(cpu->get_indirect_x_addr(), cpu->ra_); }},
    /* 0xC2 */ {"*NOP", 2, 2, nullptr},
    /* 0xC3 */ {"*DCP", 2, 8, [](NesCpu* cpu) { cpu->op_dcp(cpu->get_indirect_x_addr()); }},
    /* 0xC4 */
    {"CPY", 2, 3, [](NesCpu* cpu) { cpu->op_cmp(cpu->get_zero_page_addr(), cpu->ry_); }},
    /* 0xC5 */
    {"CMP", 2, 3, [](NesCpu* cpu) { cpu->op_cmp(cpu->get_zero_page_addr(), cpu->ra_); }},
    /* 0xC6 */ {"DEC", 2, 5, [](NesCpu* cpu) { cpu->op_dec(cpu->get_zero_page_addr()); }},
    /* 0xC7 */ {"*DCP", 2, 5, [](NesCpu* cpu) { cpu->op_dcp(cpu->get_zero_page_addr()); }},
    /* 0xC8 */ {"INY", 1, 2, [](NesCpu* cpu) { cpu->op_iny(); }},
    /* 0xC9 */
    {"CMP", 2, 2, [](NesCpu* cpu) { cpu->op_cmp(cpu->get_immediate_addr(), cpu->ra_); }},
    /* 0xCA */ {"DEX", 1, 2, [](NesCpu* cpu) { cpu->op_dex(); }},
    /* 0xCB */ {"*AXS", 2, 2, [](NesCpu* cpu) { cpu->op_axs(cpu->get_immediate_addr()); }},
    /* 0xCC */
    {"CPY", 3, 4, [](NesCpu* cpu) { cpu->op_cmp(cpu->get_absolute_addr(), cpu->ry_); }},
    /* 0xCD */
    {"CMP", 3, 4, [](NesCpu* cpu) { cpu->op_cmp(cpu->get_absolute_addr(), cpu->ra_); }},
    /* 0xCE */ {"DEC", 3, 6, [](NesCpu* cpu) { cpu->op_dec(cpu->get_absolute_addr()); }},
    /* 0xCF */ {"*DCP", 3, 6, [](NesCpu* cpu) { cpu->op_dcp(cpu->get_absolute_addr()); }},
    /* 0xD0 */
    {"BNE",
     2,
     2 /*+1 if branch succeeds, +2 if to a new page*/,
     [](NesCpu* cpu) { cpu->op_branch(!cpu->status_.z()); }},
    /* 0xD1 */
    {"CMP",
     2,
     5 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_cmp(cpu->get_indirect_y_addr(), cpu->ra_); }},
    /* 0xD2 */ {"*NOP", 1, 2, nullptr},
    /* 0xD3 */ {"*DCP", 2, 8, [](NesCpu* cpu) { cpu->op_dcp(cpu->get_indirect_y_addr()); }},
    /* 0xD4 */ {"*NOP", 2, 4, nullptr},
    /* 0xD5 */
    {"CMP", 2, 4, [](NesCpu* cpu) { cpu->op_cmp(cpu->get_zero_page_x_addr(), cpu->ra_); }},
    /* 0xD6 */ {"DEC", 2, 6, [](NesCpu* cpu) { cpu->op_dec(cpu->get_zero_page_x_addr()); }},
    /* 0xD7 */ {"*DCP", 2, 6, [](NesCpu* cpu) { cpu->op_dcp(cpu->get_zero_page_x_addr()); }},
    /* 0xD8 */
    {"CLD", 1, 2, [](NesCpu* cpu) { cpu->status_.d(0u); }},
    /* 0xD9 */
    {"CMP",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_cmp(cpu->get_absolute_y_addr(), cpu->ra_); }},
    /* 0xDA */ {"*NOP", 1, 2, nullptr},
    /* 0xDB */ {"*DCP", 3, 7, [](NesCpu* cpu) { cpu->op_dcp(cpu->get_absolute_y_addr()); }},
    /* 0xDC */ {"*NOP", 3, 4 /*+1 if page crossed*/, nullptr},
    /* 0xDD */
    {"CMP",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_cmp(cpu->get_absolute_x_addr(), cpu->ra_); }},
    /* 0xDE */ {"DEC", 3, 7, [](NesCpu* cpu) { cpu->op_dec(cpu->get_absolute_x_addr()); }},
    /* 0xDF */ {"*DCP", 3, 7, [](NesCpu* cpu) { cpu->op_dcp(cpu->get_absolute_x_addr()); }},
    /* 0xE0 */
    {"CPX", 2, 2, [](NesCpu* cpu) { cpu->op_cmp(cpu->get_immediate_addr(), cpu->rx_); }},
    /* 0xE1 */ {"SBC", 2, 6, [](NesCpu* cpu) { cpu->op_sbc(cpu->get_indirect_x_addr()); }},
    /* 0xE2 */ {"*NOP", 2, 2, nullptr},
    /* 0xE3 */ {"*ISB", 2, 8, [](NesCpu* cpu) { cpu->op_isb(cpu->get_indirect_x_addr()); }},
    /* 0xE4 */
    {"CPX", 2, 3, [](NesCpu* cpu) { cpu->op_cmp(cpu->get_zero_page_addr(), cpu->rx_); }},
    /* 0xE5 */ {"SBC", 2, 3, [](NesCpu* cpu) { cpu->op_sbc(cpu->get_zero_page_addr()); }},
    /* 0xE6 */ {"INC", 2, 5, [](NesCpu* cpu) { cpu->op_inc(cpu->get_zero_page_addr()); }},
    /* 0xE7 */ {"*ISB", 2, 5, [](NesCpu* cpu) { cpu->op_isb(cpu->get_zero_page_addr()); }},
    /* 0xE8 */ {"INX", 1, 2, [](NesCpu* cpu) { cpu->op_inx(); }},
    /* 0xE9 */ {"SBC", 2, 2, [](NesCpu* cpu) { cpu->op_sbc(cpu->get_immediate_addr()); }},
    /* 0xEA */ {"NOP", 1, 2, nullptr},
    /* 0xEB */ {"*SBC", 2, 2, [](NesCpu* cpu) { cpu->op_sbc(cpu->get_immediate_addr()); }},
    /* 0xEC */
    {"CPX", 3, 4, [](NesCpu* cpu) { cpu->op_cmp(cpu->get_absolute_addr(), cpu->rx_); }},
    /* 0xED */ {"SBC", 3, 4, [](NesCpu* cpu) { cpu->op_sbc(cpu->get_absolute_addr()); }},
    /* 0xEE */ {"INC", 3, 6, [](NesCpu* cpu) { cpu->op_inc(cpu->get_absolute_addr()); }},
    /* 0xEF */ {"*ISB", 3, 6, [](NesCpu* cpu) { cpu->op_isb(cpu->get_absolute_addr()); }},
    /* 0xF0 */
    {"BEQ",
     2,
     2 /*+1 if branch succeeds, +2 if to a new page*/,
     [](NesCpu* cpu) { cpu->op_branch(cpu->status_.z()); }},
    /* 0xF1 */
    {"SBC",
     2,
     5 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_sbc(cpu->get_indirect_y_addr()); }},
    /* 0xF2 */ {"*NOP", 1, 2, nullptr},
    /* 0xF3 */ {"*ISB", 2, 8, [](NesCpu* cpu) { cpu->op_isb(cpu->get_indirect_y_addr()); }},
    /* 0xF4 */ {"*NOP", 2, 4, nullptr},
    /* 0xF5 */ {"SBC", 2, 4, [](NesCpu* cpu) { cpu->op_sbc(cpu->get_zero_page_x_addr()); }},
    /* 0xF6 */ {"INC", 2, 6, [](NesCpu* cpu) { cpu->op_inc(cpu->get_zero_page_x_addr()); }},
    /* 0xF7 */ {"*ISB", 2, 6, [](NesCpu* cpu) { cpu->op_isb(cpu->get_zero_page_x_addr()); }},
    /* 0xF8 */
    {"SED", 1, 2, [](NesCpu* cpu) { cpu->status_.d(1u); }},
    /* 0xF9 */
    {"SBC",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_sbc(cpu->get_absolute_y_addr()); }},
    /* 0xFA */ {"*NOP", 1, 2, nullptr},
    /* 0xFB */ {"*ISB", 3, 7, [](NesCpu* cpu) { cpu->op_isb(cpu->get_absolute_y_addr()); }},
    /* 0xFC */ {"*NOP", 3, 4 /*+1 if page crossed*/, nullptr},
    /* 0xFD */
    {"SBC",
     3,
     4 /*+1 if page crossed*/,
     [](NesCpu* cpu) { cpu->op_sbc(cpu->get_absolute_x_addr()); }},
    /* 0xFE */ {"INC", 3, 7, [](NesCpu* cpu) { cpu->op_inc(cpu->get_absolute_x_addr()); }},
    /* 0xFF */ {"*ISB", 3, 7, [](NesCpu* cpu) { cpu->op_isb(cpu->get_absolute_x_addr()); }},
}};

NesCpu::NesCpu(const std::shared_ptr<NesBus>& bus)
    : ra_(0u)
    , rx_(0u)
    , ry_(0u)
    , sp_(STACK_RESET)
    , pc_(0x8000)
    , status_()
    , bus_(bus)
{
}

void NesCpu::reset()
{
    ra_     = 0u;
    rx_     = 0u;
    ry_     = 0u;
    status_ = NesCpuStatus();
    sp_     = STACK_RESET;

    pc_ = bus_->read16(0xFFFC);
}

bool NesCpu::step()
{
    uint8_t code      = bus_->read8(pc_++);
    const OpCode& op  = OPCODES.at(code);
    uint16_t pc_state = pc_;

    if (code == 0x00)
        return true;

    if (!op.callback)
        return true;

    op.callback(this);

    if (pc_state == pc_)
        pc_ += (op.length - 1u);

    return false;
}

void NesCpu::stack_push8(uint8_t value)
{
    bus_->write8(STACK_BASE + sp_, value);
    sp_--;
}

void NesCpu::stack_push16(uint16_t value)
{
    stack_push8(static_cast<uint8_t>(value >> 8u));
    stack_push8(static_cast<uint8_t>(value & 0xFF));
}

uint8_t NesCpu::stack_pop8()
{
    sp_++;
    return bus_->read8(STACK_BASE + sp_);
}

uint16_t NesCpu::stack_pop16()
{
    uint16_t lo = stack_pop8();
    uint16_t hi = stack_pop8();
    return (hi << 8u) | lo;
}

uint16_t NesCpu::get_immediate_addr() const
{
    return pc_;
}

uint16_t NesCpu::get_zero_page_addr() const
{
    return bus_->read8(pc_);
}

uint16_t NesCpu::get_zero_page_x_addr() const
{
    return static_cast<uint8_t>(bus_->read8(pc_) + rx_);
}

uint16_t NesCpu::get_zero_page_y_addr() const
{
    return static_cast<uint8_t>(bus_->read8(pc_) + ry_);
}

uint16_t NesCpu::get_absolute_addr() const
{
    return bus_->read16(pc_);
}

uint16_t NesCpu::get_absolute_x_addr() const
{
    return bus_->read16(pc_) + rx_;
}

uint16_t NesCpu::get_absolute_y_addr() const
{
    return bus_->read16(pc_) + ry_;
}

uint16_t NesCpu::get_indirect_x_addr() const
{
    uint8_t base = bus_->read8(pc_) + rx_;
    return bus_->read8(base) | (bus_->read8(static_cast<uint8_t>(base + 1u)) << 8u);
}

uint16_t NesCpu::get_indirect_y_addr() const
{
    uint16_t base  = bus_->read8(pc_);
    uint16_t deref = bus_->read8(base & 0xFF) | (bus_->read8((base + 1u) & 0xFF) << 8u);
    return deref + ry_;
}

void NesCpu::add_to_ra(uint8_t value)
{
    uint16_t sum = ra_ + value + status_.c();
    status_.c(sum > 0xFF);
    uint8_t result = static_cast<uint8_t>(sum);
    status_.v((value ^ result) & (result ^ ra_) & 0x80);
    ra_ = result;
    status_.set_nz(ra_);
}

void NesCpu::op_adc(uint16_t addr)
{
    uint8_t value = bus_->read8(addr);
    add_to_ra(value);
}

void NesCpu::op_ahx(uint16_t addr)
{
    uint8_t value = ra_ & rx_ & static_cast<uint8_t>(addr >> 8u);
    bus_->write8(addr, value);
}

void NesCpu::op_alr(uint16_t addr)
{
    ra_ &= bus_->read8(addr);
    status_.c(ra_ & 0x01);
    ra_ >>= 1u;
    status_.set_nz(ra_);
}

void NesCpu::op_anc(uint16_t addr)
{
    ra_ &= bus_->read8(addr);
    status_.set_nz(ra_);
    status_.c(status_.n());
}

void NesCpu::op_and(uint16_t addr)
{
    ra_ &= bus_->read8(addr);
    status_.set_nz(ra_);
}

void NesCpu::op_arr(uint16_t addr)
{
    ra_ &= bus_->read8(addr);
    bool old_carry = status_.c();
    status_.c(ra_ & 0x01);
    ra_ >>= 1u;
    if (old_carry)
        ra_ |= 0x80;
    bool bit_5 = (ra_ >> 5u) & 0x1;
    bool bit_6 = (ra_ >> 6u) & 0x1;
    status_.c(bit_6);
    status_.v((bit_5 ^ bit_6) == 0x1);
    status_.set_nz(ra_);
}

void NesCpu::op_asl_accumulator()
{
    status_.c((ra_ >> 7u) & 0x1);
    ra_ <<= 1u;
    status_.set_nz(ra_);
}

void NesCpu::op_asl(uint16_t addr)
{
    uint8_t value = bus_->read8(addr);
    status_.c((value >> 7u) & 0x1);
    value <<= 1u;
    bus_->write8(addr, value);
    status_.set_nz(value);
}

void NesCpu::op_axs(uint16_t addr)
{
    uint8_t value   = bus_->read8(addr);
    uint8_t a_and_x = ra_ & rx_;
    uint8_t result  = a_and_x - value;
    status_.c(value <= a_and_x);
    rx_ = result;
    status_.set_nz(result);
}

void NesCpu::op_bit(uint16_t addr)
{
    uint8_t value = bus_->read8(addr);
    status_.z(!(ra_ & value));
    status_.v((value >> 6u) & 0x1);
    status_.n((value >> 7u) & 0x1);
}

void NesCpu::op_branch(bool condition)
{
    if (condition)
    {
        int8_t jump = static_cast<int8_t>(bus_->read8(pc_));
        pc_ += static_cast<uint16_t>(jump) + 1u;
    }
}

void NesCpu::op_cmp(uint16_t addr, uint8_t compare_with)
{
    uint8_t value = bus_->read8(addr);
    status_.c(compare_with >= value);
    status_.set_nz(compare_with - value);
}

void NesCpu::op_dcp(uint16_t addr)
{
    uint8_t value = bus_->read8(addr);
    value--;
    bus_->write8(addr, value);
    status_.c(value <= ra_);
    status_.set_nz(ra_ - value);
}

void NesCpu::op_dec(uint16_t addr)
{
    uint8_t value = bus_->read8(addr);
    value--;
    bus_->write8(addr, value);
    status_.set_nz(value);
}

void NesCpu::op_dex()
{
    rx_--;
    status_.set_nz(rx_);
}

void NesCpu::op_dey()
{
    ry_--;
    status_.set_nz(ry_);
}

void NesCpu::op_eor(uint16_t addr)
{
    ra_ ^= bus_->read8(addr);
    status_.set_nz(ra_);
}

void NesCpu::op_inc(uint16_t addr)
{
    uint8_t value = bus_->read8(addr);
    value++;
    bus_->write8(addr, value);
    status_.set_nz(value);
}

void NesCpu::op_inx()
{
    rx_++;
    status_.set_nz(rx_);
}

void NesCpu::op_iny()
{
    ry_++;
    status_.set_nz(ry_);
}

void NesCpu::op_isb(uint16_t addr)
{
    uint8_t value = bus_->read8(addr);
    value++;
    bus_->write8(addr, value);
    status_.set_nz(value);
    add_to_ra(~value);
}

void NesCpu::op_jmp_absolute()
{
    pc_ = bus_->read16(pc_);
}

void NesCpu::op_jmp_indirect()
{
    uint16_t addr = bus_->read16(pc_);

    if ((addr & 0x00FF) == 0x00FF)
    {
        uint16_t lo = bus_->read8(addr);
        uint16_t hi = bus_->read8(addr & 0xFF00);
        pc_         = (hi << 8u) | lo;
    }
    else
    {
        pc_ = bus_->read16(addr);
    }
}

void NesCpu::op_jsr()
{
    stack_push16(pc_ + 2u - 1u);
    pc_ = bus_->read16(pc_);
}

void NesCpu::op_las(uint16_t addr)
{
    sp_ &= bus_->read8(addr);
    ra_ = sp_;
    rx_ = sp_;
    status_.set_nz(sp_);
}

void NesCpu::op_lax(uint16_t addr)
{
    uint8_t value = bus_->read8(addr);
    ra_           = value;
    rx_           = value;
    status_.set_nz(ra_);
}

void NesCpu::op_lda(uint16_t addr)
{
    ra_ = bus_->read8(addr);
    status_.set_nz(ra_);
}

void NesCpu::op_ldx(uint16_t addr)
{
    rx_ = bus_->read8(addr);
    status_.set_nz(rx_);
}

void NesCpu::op_ldy(uint16_t addr)
{
    ry_ = bus_->read8(addr);
    status_.set_nz(ry_);
}

void NesCpu::op_lsr_accumulator()
{
    status_.c(ra_ & 0x1);
    ra_ >>= 1u;
    status_.set_nz(ra_);
}

void NesCpu::op_lsr(uint16_t addr)
{
    uint8_t value = bus_->read8(addr);
    status_.c(value & 0x1);
    value >>= 1;
    bus_->write8(addr, value);
    status_.set_nz(value);
}

void NesCpu::op_lxa(uint16_t addr)
{
    ra_ = bus_->read8(addr);
    rx_ = ra_;
    status_.set_nz(rx_);
}

void NesCpu::op_ora(uint16_t addr)
{
    ra_ |= bus_->read8(addr);
    status_.set_nz(ra_);
}

void NesCpu::op_php()
{
    NesCpuStatus status = status_;
    status.b(1u);
    status.u(1u);
    stack_push8(status.get());
}

void NesCpu::op_pla()
{
    ra_ = stack_pop8();
    status_.set_nz(ra_);
}

void NesCpu::op_plp()
{
    status_.set(stack_pop8());
    status_.b(0u);
    status_.u(1u);
}

void NesCpu::op_rla(uint16_t addr)
{
    uint8_t value  = bus_->read8(addr);
    bool old_carry = status_.c();
    status_.c((value >> 7u) & 0x1);
    value <<= 1u;
    if (old_carry)
        value |= 0x1;
    bus_->write8(addr, value);
    ra_ &= value;
    status_.set_nz(ra_);
}

void NesCpu::op_rol_accumulator()
{
    bool old_carry = status_.c();
    status_.c((ra_ >> 7u) & 0x1);
    ra_ <<= 1u;
    if (old_carry)
        ra_ |= 0x1;
    status_.set_nz(ra_);
}

void NesCpu::op_rol(uint16_t addr)
{
    uint8_t value  = bus_->read8(addr);
    bool old_carry = status_.c();
    status_.c((value >> 7u) & 0x1);
    value <<= 1u;
    if (old_carry)
        value |= 0x1;
    bus_->write8(addr, value);
    status_.set_nz(value);
}

void NesCpu::op_ror_accumulator()
{
    bool old_carry = status_.c();
    status_.c(ra_ & 0x1);
    ra_ >>= 1u;
    if (old_carry)
        ra_ |= 0x80;
    status_.set_nz(ra_);
}

void NesCpu::op_ror(uint16_t addr)
{
    uint8_t value  = bus_->read8(addr);
    bool old_carry = status_.c();
    status_.c(value & 0x1);
    value >>= 1u;
    if (old_carry)
        value |= 0x80;
    bus_->write8(addr, value);
    status_.set_nz(value);
}

void NesCpu::op_rra(uint16_t addr)
{
    uint8_t value  = bus_->read8(addr);
    bool old_carry = status_.c();
    status_.c(value & 0x1);
    value >>= 1u;
    if (old_carry)
        value |= 0x80;
    bus_->write8(addr, value);
    add_to_ra(value);
}

void NesCpu::op_rti()
{
    status_.set(stack_pop8());
    status_.b(0u);
    status_.u(1u);
    pc_ = stack_pop16();
}

void NesCpu::op_rts()
{
    pc_ = stack_pop16() + 1u;
}

void NesCpu::op_sax(uint16_t addr)
{
    uint8_t value = ra_ & rx_;
    bus_->write8(addr, value);
}

void NesCpu::op_sbc(uint16_t addr)
{
    uint8_t value = bus_->read8(addr);
    add_to_ra(~value);
}

void NesCpu::op_shx(uint16_t addr)
{
    uint8_t value = rx_ & (static_cast<uint8_t>(addr >> 8u) + 1u);
    bus_->write8(addr, value);
}

void NesCpu::op_shy(uint16_t addr)
{
    uint8_t value = ry_ & (static_cast<uint8_t>(addr >> 8u) + 1u);
    bus_->write8(addr, value);
}

void NesCpu::op_slo(uint16_t addr)
{
    uint8_t value = bus_->read8(addr);
    status_.c((value >> 7u) & 0x1);
    value <<= 1u;
    bus_->write8(addr, value);
    ra_ |= value;
    status_.set_nz(ra_);
}

void NesCpu::op_sre(uint16_t addr)
{
    uint8_t value = bus_->read8(addr);
    status_.c(value & 0x1);
    value >>= 1u;
    bus_->write8(addr, value);
    ra_ ^= value;
    status_.set_nz(ra_);
}

void NesCpu::op_sta(uint16_t addr)
{
    bus_->write8(addr, ra_);
}

void NesCpu::op_stx(uint16_t addr)
{
    bus_->write8(addr, rx_);
}

void NesCpu::op_sty(uint16_t addr)
{
    bus_->write8(addr, ry_);
}

void NesCpu::op_tas(uint16_t addr)
{
    sp_           = ra_ & rx_;
    uint8_t value = (static_cast<uint8_t>(addr >> 8u) + 1u) & sp_;
    bus_->write8(addr, value);
}

void NesCpu::op_tax()
{
    rx_ = ra_;
    status_.set_nz(rx_);
}

void NesCpu::op_tay()
{
    ry_ = ra_;
    status_.set_nz(ry_);
}

void NesCpu::op_tsx()
{
    rx_ = sp_;
    status_.set_nz(rx_);
}

void NesCpu::op_txa()
{
    ra_ = rx_;
    status_.set_nz(ra_);
}

void NesCpu::op_txs()
{
    sp_ = rx_;
}

void NesCpu::op_tya()
{
    ra_ = ry_;
    status_.set_nz(ra_);
}

void NesCpu::op_xaa(uint16_t addr)
{
    ra_ = rx_;
    ra_ &= bus_->read8(addr);
    status_.set_nz(ra_);
}
