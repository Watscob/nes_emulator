#include "cpu.hpp"
#include <stdexcept>
#include "bus.hpp"
#include "log.hpp"
#include "opcodes.hpp"

Cpu::Cpu(std::shared_ptr<Bus> bus)
    : register_a_(0u)
    , register_x_(0u)
    , register_y_(0u)
    , status_()
    , program_counter_(0x8000)
    , stack_pointer_(STACK_RESET)
    , bus_(bus)
{
}

void Cpu::reset()
{
    register_a_    = 0u;
    register_x_    = 0u;
    register_y_    = 0u;
    status_        = CpuStatus();
    stack_pointer_ = STACK_RESET;

    program_counter_ = bus_->read16(0xFFFC);
}

bool Cpu::step()
{
    uint8_t code = bus_->read8(program_counter_++);
    OpCode opcode;

    try
    {
        opcode = OPCODES.at(code);
    }
    catch (const std::out_of_range&)
    {
        log_error("Code {:#04x} does not exists.", code);
        return false;
    }

    uint16_t program_counter_state = program_counter_;

    switch (code)
    {
    /* BRK */
    case 0x00:
        return false;
    /* ADC */
    case 0x61:
    case 0x65:
    case 0x69:
    case 0x6D:
    case 0x71:
    case 0x75:
    case 0x79:
    case 0x7D:
        op_adc(opcode.mode);
        break;
    /* AND */
    case 0x21:
    case 0x25:
    case 0x29:
    case 0x2D:
    case 0x31:
    case 0x35:
    case 0x39:
    case 0x3D:
        op_and(opcode.mode);
        break;
    /* ASL accumulator */
    case 0x0A:
        op_asl_accumulator();
        break;
    /* ASL */
    case 0x06:
    case 0x0E:
    case 0x16:
    case 0x1E:
        op_asl(opcode.mode);
        break;
    /* BCC */
    case 0x90:
        op_branch(!status_.get_carry());
        break;
    /* BCS */
    case 0xB0:
        op_branch(status_.get_carry());
        break;
    /* BEQ */
    case 0xF0:
        op_branch(status_.get_zero());
        break;
    /* BIT */
    case 0x24:
    case 0x2C:
        op_bit(opcode.mode);
        break;
    /* BMI */
    case 0x30:
        op_branch(status_.get_negative());
        break;
    /* BNE */
    case 0xD0:
        op_branch(!status_.get_zero());
        break;
    /* BPL */
    case 0x10:
        op_branch(!status_.get_negative());
        break;
    /* BVC */
    case 0x50:
        op_branch(!status_.get_overflow());
        break;
    /* BVS */
    case 0x70:
        op_branch(status_.get_overflow());
        break;
    /* CLC */
    case 0x18:
        status_.set_carry(0);
        break;
    /* CLD */
    case 0xD8:
        status_.set_decimal(0);
        break;
    /* CLI */
    case 0x58:
        status_.set_interrupt(0);
        break;
    /* CLV */
    case 0xB8:
        status_.set_overflow(0);
        break;
    /* CMP */
    case 0xC1:
    case 0xC5:
    case 0xC9:
    case 0xCD:
    case 0xD1:
    case 0xD5:
    case 0xD9:
    case 0xDD:
        op_cmp(opcode.mode, register_a_);
        break;
    /* CPX */
    case 0xE0:
    case 0xE4:
    case 0xEC:
        op_cmp(opcode.mode, register_x_);
        break;
    /* CPY */
    case 0xC0:
    case 0xC4:
    case 0xCC:
        op_cmp(opcode.mode, register_y_);
        break;
    /* *DCP */
    case 0xC3:
    case 0xC7:
    case 0xCF:
    case 0xD3:
    case 0xD7:
    case 0xDB:
    case 0xDF:
        op_dcp(opcode.mode);
        break;
    /* DEC */
    case 0xC6:
    case 0xCE:
    case 0xD6:
    case 0xDE:
        op_dec(opcode.mode);
        break;
    /* DEX */
    case 0xCA:
        op_dex();
        break;
    /* DEY */
    case 0x88:
        op_dey();
        break;
    /* EOR */
    case 0x41:
    case 0x45:
    case 0x49:
    case 0x4D:
    case 0x51:
    case 0x55:
    case 0x59:
    case 0x5D:
        op_eor(opcode.mode);
        break;
    /* INC */
    case 0xE6:
    case 0xEE:
    case 0xF6:
    case 0xFE:
        op_inc(opcode.mode);
        break;
    /* INX */
    case 0xE8:
        op_inx();
        break;
    /* INY */
    case 0xC8:
        op_iny();
        break;
    /* ISB */
    case 0xE3:
    case 0xE7:
    case 0xEF:
    case 0xF3:
    case 0xF7:
    case 0xFB:
    case 0xFF:
        op_isb(opcode.mode);
        break;
    /* JMP absolute */
    case 0x4C:
        op_jmp_absolute();
        break;
    /* JMP indirect */
    case 0x6C:
        op_jmp_indirect();
        break;
    /* JSR */
    case 0x20:
        op_jsr();
        break;
    /* LAX */
    case 0xA3:
    case 0xA7:
    case 0xAF:
    case 0xB3:
    case 0xB7:
    case 0xBF:
        op_lax(opcode.mode);
        break;
    /* LDA */
    case 0xA1:
    case 0xA5:
    case 0xA9:
    case 0xAD:
    case 0xB1:
    case 0xB5:
    case 0xB9:
    case 0xBD:
        op_lda(opcode.mode);
        break;
    /* LDX */
    case 0xA2:
    case 0xA6:
    case 0xAE:
    case 0xB6:
    case 0xBE:
        op_ldx(opcode.mode);
        break;
    /* LDY */
    case 0xA0:
    case 0xA4:
    case 0xAC:
    case 0xB4:
    case 0xBC:
        op_ldy(opcode.mode);
        break;
    /* LSR accumulator */
    case 0x4A:
        op_lsr_accumulator();
        break;
    /* LSR */
    case 0x46:
    case 0x4E:
    case 0x56:
    case 0x5E:
        op_lsr(opcode.mode);
        break;
    /* NOP */
    case 0xEA:
        break;
    /* ORA */
    case 0x01:
    case 0x05:
    case 0x09:
    case 0x0D:
    case 0x11:
    case 0x15:
    case 0x19:
    case 0x1D:
        op_ora(opcode.mode);
        break;
    /* PHA */
    case 0x48:
        stack_push8(register_a_);
        break;
    /* PHP */
    case 0x08:
        op_php();
        break;
    /* PLA */
    case 0x68:
        op_pla();
        break;
    /* PLP */
    case 0x28:
        op_plp();
        break;
    /* RLA */
    case 0x23:
    case 0x27:
    case 0x2F:
    case 0x33:
    case 0x37:
    case 0x3B:
    case 0x3F:
        op_rla(opcode.mode);
        break;
    /* ROL accumulator */
    case 0x2A:
        op_rol_accumulator();
        break;
    /* ROL */
    case 0x26:
    case 0x2E:
    case 0x36:
    case 0x3E:
        op_rol(opcode.mode);
        break;
    /* ROR accumulator */
    case 0x6A:
        op_ror_accumulator();
        break;
    /* ROR */
    case 0x66:
    case 0x6E:
    case 0x76:
    case 0x7E:
        op_ror(opcode.mode);
        break;
    /* RRA */
    case 0x63:
    case 0x67:
    case 0x6F:
    case 0x73:
    case 0x77:
    case 0x7B:
    case 0x7F:
        op_rra(opcode.mode);
        break;
    /* RTI */
    case 0x40:
        op_rti();
        break;
    /* RTS */
    case 0x60:
        op_rts();
        break;
    /* SAX */
    case 0x83:
    case 0x87:
    case 0x8F:
    case 0x97:
        op_sax(opcode.mode);
        break;
    /* SBC */
    case 0xE1:
    case 0xE5:
    case 0xE9:
    case 0xEB:
    case 0xED:
    case 0xF1:
    case 0xF5:
    case 0xF9:
    case 0xFD:
        op_sbc(opcode.mode);
        break;
    /* SEC */
    case 0x38:
        status_.set_carry(1);
        break;
    /* SED */
    case 0xF8:
        status_.set_decimal(1);
        break;
    /* SEI */
    case 0x78:
        status_.set_interrupt(1);
        break;
    /* SLO */
    case 0x03:
    case 0x07:
    case 0x0F:
    case 0x13:
    case 0x17:
    case 0x1B:
    case 0x1F:
        op_slo(opcode.mode);
        break;
    /* SRE */
    case 0x43:
    case 0x47:
    case 0x4F:
    case 0x53:
    case 0x57:
    case 0x5B:
    case 0x5F:
        op_sre(opcode.mode);
        break;
    /* STA */
    case 0x81:
    case 0x85:
    case 0x8D:
    case 0x91:
    case 0x95:
    case 0x99:
    case 0x9D:
        op_sta(opcode.mode);
        break;
    /* STX */
    case 0x86:
    case 0x8E:
    case 0x96:
        op_stx(opcode.mode);
        break;
    /* STY */
    case 0x84:
    case 0x8C:
    case 0x94:
        op_sty(opcode.mode);
        break;
    /* TAX */
    case 0xAA:
        op_tax();
        break;
    /* TAY */
    case 0xA8:
        op_tay();
        break;
    /* TSX */
    case 0xBA:
        op_tsx();
        break;
    /* TXA */
    case 0x8A:
        op_txa();
        break;
    /* TXS */
    case 0x9A:
        op_txs();
        break;
    /* TYA */
    case 0x98:
        op_tya();
        break;
    /* UNKNOWN */
    default:
        log_error("Code {} ({:#04x}) is not implemented.", opcode.name, code);
        break;
    }

    if (program_counter_state == program_counter_)
        program_counter_ += (opcode.len - 1u);

    return true;
}

uint16_t Cpu::get_absolute_address(AddressingMode mode, uint16_t addr) const
{
    switch (mode)
    {
    case AddressingMode::ZERO_PAGE:
        return bus_->read8(addr);
    case AddressingMode::ABSOLUTE:
        return bus_->read16(addr);
    case AddressingMode::ZERO_PAGE_X:
        return static_cast<uint8_t>(bus_->read8(addr) + register_x_);
    case AddressingMode::ZERO_PAGE_Y:
        return static_cast<uint8_t>(bus_->read8(addr) + register_y_);
    case AddressingMode::ABSOLUTE_X:
        return bus_->read16(addr) + register_x_;
    case AddressingMode::ABSOLUTE_Y:
        return bus_->read16(addr) + register_y_;
    case AddressingMode::INDIRECT_X:
    {
        uint8_t base = bus_->read8(addr) + register_x_;
        return bus_->read8(base) | (bus_->read8(static_cast<uint8_t>(base + 1)) << 8);
    }
    case AddressingMode::INDIRECT_Y:
    {
        uint16_t base  = bus_->read8(addr);
        uint16_t deref = bus_->read8(base & 0xFF) | (bus_->read8((base + 1) & 0xFF) << 8);
        return deref + register_y_;
    }
    default:
        log_error("Mode {} is not supproted.", static_cast<int>(mode));
        return 0u;
    }
}

uint16_t Cpu::get_operand_address(AddressingMode mode) const
{
    switch (mode)
    {
    case AddressingMode::IMMEDIATE:
        return program_counter_;
    default:
        return get_absolute_address(mode, program_counter_);
    }
}

void Cpu::stack_push8(uint8_t data)
{
    bus_->write8(STACK_BASE + stack_pointer_, data);
    stack_pointer_--;
}

uint8_t Cpu::stack_pop8()
{
    stack_pointer_++;
    return bus_->read8(STACK_BASE + stack_pointer_);
}

void Cpu::stack_push16(uint16_t data)
{
    stack_push8(static_cast<uint8_t>(data >> 8));
    stack_push8(static_cast<uint8_t>(data & 0xFF));
}

uint16_t Cpu::stack_pop16()
{
    uint16_t lo = stack_pop8();
    uint16_t hi = stack_pop8();
    return (hi << 8) | lo;
}

void Cpu::add_to_register_a(uint8_t value)
{
    uint16_t sum = register_a_;
    sum += value;
    sum += status_.get_carry();
    status_.set_carry(sum > 0xFF);
    uint8_t result = static_cast<uint8_t>(sum);
    status_.set_overflow(((value ^ result) & (result ^ register_a_) & 0x80));
    register_a_ = result;
    update_zero_and_negative(register_a_);
}

void Cpu::op_adc(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = bus_->read8(addr);
    add_to_register_a(value);
}

void Cpu::op_and(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    register_a_ &= bus_->read8(addr);
    update_zero_and_negative(register_a_);
}

void Cpu::op_asl_accumulator()
{
    status_.set_carry((register_a_ >> 7) & 0x1);
    register_a_ <<= 1;
    update_zero_and_negative(register_a_);
}

void Cpu::op_asl(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = bus_->read8(addr);
    status_.set_carry((value >> 7) & 0x1);
    value <<= 1;
    bus_->write8(addr, value);
    update_zero_and_negative(value);
}

void Cpu::op_branch(bool condition)
{
    if (condition)
    {
        std::int8_t jump = static_cast<std::int8_t>(bus_->read8(program_counter_));
        program_counter_ += static_cast<uint16_t>(jump) + 1;
    }
}

void Cpu::op_bit(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = bus_->read8(addr);
    status_.set_zero(!(register_a_ & value));
    status_.set_overflow((value >> 6) & 0x1);
    status_.set_negative((value >> 7) & 0x1);
}

void Cpu::op_cmp(AddressingMode mode, uint8_t compare_with)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = bus_->read8(addr);
    status_.set_carry(compare_with >= value);
    update_zero_and_negative(compare_with - value);
}

void Cpu::op_dcp(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = bus_->read8(addr);
    value--;
    bus_->write8(addr, value);
    status_.set_carry(value <= register_a_);
    update_zero_and_negative(register_a_ - value);
}

void Cpu::op_dec(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = bus_->read8(addr);
    value--;
    bus_->write8(addr, value);
    update_zero_and_negative(value);
}

void Cpu::op_dex()
{
    register_x_--;
    update_zero_and_negative(register_x_);
}

void Cpu::op_dey()
{
    register_y_--;
    update_zero_and_negative(register_y_);
}

void Cpu::op_eor(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    register_a_ ^= bus_->read8(addr);
    update_zero_and_negative(register_a_);
}

void Cpu::op_inc(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = bus_->read8(addr);
    value++;
    bus_->write8(addr, value);
    update_zero_and_negative(value);
}

void Cpu::op_inx()
{
    register_x_++;
    update_zero_and_negative(register_x_);
}

void Cpu::op_iny()
{
    register_y_++;
    update_zero_and_negative(register_y_);
}

void Cpu::op_isb(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = bus_->read8(addr);
    value++;
    bus_->write8(addr, value);
    update_zero_and_negative(value);
    add_to_register_a(~value);
}

void Cpu::op_jmp_absolute()
{
    program_counter_ = bus_->read16(program_counter_);
}

void Cpu::op_jmp_indirect()
{
    uint16_t addr = bus_->read16(program_counter_);

    if ((addr & 0x00FF) == 0x00FF)
    {
        uint16_t lo      = bus_->read8(addr);
        uint16_t hi      = bus_->read8(addr & 0xFF00);
        program_counter_ = (hi << 8) | lo;
    }
    else
    {
        program_counter_ = bus_->read16(addr);
    }
}

void Cpu::op_jsr()
{
    stack_push16(program_counter_ + 2 - 1);
    program_counter_ = bus_->read16(program_counter_);
}

void Cpu::op_lax(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = bus_->read8(addr);
    register_a_   = value;
    register_x_   = value;
    update_zero_and_negative(register_a_);
}

void Cpu::op_lda(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    register_a_   = bus_->read8(addr);
    update_zero_and_negative(register_a_);
}

void Cpu::op_ldx(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    register_x_   = bus_->read8(addr);
    update_zero_and_negative(register_x_);
}

void Cpu::op_ldy(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    register_y_   = bus_->read8(addr);
    update_zero_and_negative(register_y_);
}

void Cpu::op_lsr_accumulator()
{
    status_.set_carry(register_a_ & 0x1);
    register_a_ >>= 1;
    update_zero_and_negative(register_a_);
}

void Cpu::op_lsr(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = bus_->read8(addr);
    status_.set_carry(value & 0x1);
    value >>= 1;
    bus_->write8(addr, value);
    update_zero_and_negative(value);
}

void Cpu::op_ora(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    register_a_ |= bus_->read8(addr);
    update_zero_and_negative(register_a_);
}

void Cpu::op_php()
{
    CpuStatus status = status_;
    status.set_break(1);
    status.set_unused(1);
    stack_push8(status);
}

void Cpu::op_pla()
{
    register_a_ = stack_pop8();
    update_zero_and_negative(register_a_);
}

void Cpu::op_plp()
{
    status_.set(stack_pop8());
    status_.set_break(0);
    status_.set_unused(1);
}

void Cpu::op_rla(AddressingMode mode)
{
    uint16_t addr  = get_operand_address(mode);
    uint8_t value  = bus_->read8(addr);
    bool old_carry = status_.get_carry();
    status_.set_carry((value >> 7) & 0x1);
    value <<= 1;
    if (old_carry)
        value |= 0x1;
    bus_->write8(addr, value);
    register_a_ &= value;
    update_zero_and_negative(register_a_);
}

void Cpu::op_rol_accumulator()
{
    bool old_carry = status_.get_carry();
    status_.set_carry((register_a_ >> 7) & 0x1);
    register_a_ <<= 1;
    if (old_carry)
        register_a_ |= 0x1;
    update_zero_and_negative(register_a_);
}

void Cpu::op_rol(AddressingMode mode)
{
    uint16_t addr  = get_operand_address(mode);
    uint8_t value  = bus_->read8(addr);
    bool old_carry = status_.get_carry();
    status_.set_carry((value >> 7) & 0x1);
    value <<= 1;
    if (old_carry)
        value |= 0x1;
    bus_->write8(addr, value);
    update_zero_and_negative(value);
}

void Cpu::op_ror_accumulator()
{
    bool old_carry = status_.get_carry();
    status_.set_carry(register_a_ & 0x1);
    register_a_ >>= 1;
    if (old_carry)
        register_a_ |= 0x80;
    update_zero_and_negative(register_a_);
}

void Cpu::op_ror(AddressingMode mode)
{
    uint16_t addr  = get_operand_address(mode);
    uint8_t value  = bus_->read8(addr);
    bool old_carry = status_.get_carry();
    status_.set_carry(value & 0x1);
    value >>= 1;
    if (old_carry)
        value |= 0x80;
    bus_->write8(addr, value);
    update_zero_and_negative(value);
}

void Cpu::op_rra(AddressingMode mode)
{
    uint16_t addr  = get_operand_address(mode);
    uint8_t value  = bus_->read8(addr);
    bool old_carry = status_.get_carry();
    status_.set_carry(value & 0x1);
    value >>= 1;
    if (old_carry)
        value |= 0x80;
    bus_->write8(addr, value);
    add_to_register_a(value);
}

void Cpu::op_rti()
{
    status_.set(stack_pop8());
    status_.set_break(0);
    status_.set_unused(1);
    program_counter_ = stack_pop16();
}

void Cpu::op_rts()
{
    program_counter_ = stack_pop16() + 1;
}

void Cpu::op_sax(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = register_a_ & register_x_;
    bus_->write8(addr, value);
}

void Cpu::op_sbc(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = bus_->read8(addr);
    add_to_register_a(~value);
}

void Cpu::op_slo(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = bus_->read8(addr);
    status_.set_carry((value >> 7) & 0x1);
    value <<= 1;
    bus_->write8(addr, value);
    register_a_ |= value;
    update_zero_and_negative(register_a_);
}

void Cpu::op_sre(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    uint8_t value = bus_->read8(addr);
    status_.set_carry(value & 0x1);
    value >>= 1;
    bus_->write8(addr, value);
    register_a_ ^= value;
    update_zero_and_negative(register_a_);
}

void Cpu::op_sta(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    bus_->write8(addr, register_a_);
}

void Cpu::op_stx(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    bus_->write8(addr, register_x_);
}

void Cpu::op_sty(AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    bus_->write8(addr, register_y_);
}

void Cpu::op_tax()
{
    register_x_ = register_a_;
    update_zero_and_negative(register_x_);
}

void Cpu::op_tay()
{
    register_y_ = register_a_;
    update_zero_and_negative(register_y_);
}

void Cpu::op_tsx()
{
    register_x_ = stack_pointer_;
    update_zero_and_negative(register_x_);
}

void Cpu::op_txa()
{
    register_a_ = register_x_;
    update_zero_and_negative(register_a_);
}

void Cpu::op_txs()
{
    stack_pointer_ = register_x_;
}

void Cpu::op_tya()
{
    register_a_ = register_y_;
    update_zero_and_negative(register_a_);
}
