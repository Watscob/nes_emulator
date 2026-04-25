#include "cpu.hpp"
#include <stdexcept>
#include "log.hpp"

void Cpu::load_and_run(std::vector<std::uint8_t> rom)
{
    load(rom);
    reset();
    run();
}

void Cpu::load(std::vector<std::uint8_t> rom)
{
    memory_->load(0x8000, rom);
    memory_->write16(0xFFFC, 0x8000);
}

void Cpu::reset()
{
    register_a_ = 0u;
    register_x_ = 0u;
    register_y_ = 0u;
    status_ = 0u;

    program_counter_ = memory_->read16(0xFFFC);
}

void Cpu::run()
{
    while (1)
    {
        std::uint8_t code = memory_->read8(program_counter_++);
        OpCode opcode;

        try
        {
            opcode = OPCODES.at(code);
        }
        catch (const std::out_of_range&)
        {
            log_error("Code {:#04x} does not exists.", code);
            continue;
        }

        std::uint16_t program_counter_state = program_counter_;

        switch (code)
        {
        /* BRK */
        case 0x00:
            return;
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
        /* CLC */
        case 0x18:
            set_carry(0);
            break;
        /* CLD */
        case 0xD8:
            set_decimal(0);
            break;
        /* CLI */
        case 0x58:
            set_interrupt(0);
            break;
        /* CLV */
        case 0xB8:
            set_overflow(0);
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
        /* TAX */
        case 0xAA:
            op_tax();
            break;
        /* UNKNOWN */
        default:
            log_error("Code {} ({:#04x}) is not implemented.", opcode.name, code);
            break;
        }

        if (program_counter_state == program_counter_)
            program_counter_ += static_cast<std::uint16_t>(opcode.len - 1);
    }
}

std::uint16_t Cpu::get_operand_address(AddressingMode mode)
{
    switch (mode)
    {
    case AddressingMode::IMMEDIATE:
        return program_counter_;
    case AddressingMode::ZERO_PAGE:
        return static_cast<std::uint16_t>(memory_->read8(program_counter_));
    case AddressingMode::ABSOLUTE:
        return memory_->read16(program_counter_);
    case AddressingMode::ZERO_PAGE_X:
    {
        std::uint16_t addr = static_cast<std::uint16_t>(memory_->read8(program_counter_));
        addr += static_cast<std::uint16_t>(register_x_);
        return addr;
    }
    case AddressingMode::ZERO_PAGE_Y:
    {
        std::uint16_t addr = static_cast<std::uint16_t>(memory_->read8(program_counter_));
        addr += static_cast<std::uint16_t>(register_y_);
        return addr;
    }
    case AddressingMode::ABSOLUTE_X:
    {
        std::uint16_t addr = memory_->read16(program_counter_);
        addr += static_cast<std::uint16_t>(register_x_);
        return addr;
    }
    case AddressingMode::ABSOLUTE_Y:
    {
        std::uint16_t addr = memory_->read16(program_counter_);
        addr += static_cast<std::uint16_t>(register_y_);
        return addr;
    }
    case AddressingMode::INDIRECT_X:
    {
        std::uint8_t addr = memory_->read8(program_counter_);
        addr += register_x_;
        return memory_->read16(static_cast<std::uint16_t>(addr));
    }
    case AddressingMode::INDIRECT_Y:
    {
        std::uint16_t addr = static_cast<std::uint16_t>(memory_->read8(program_counter_));
        std::uint16_t deref = memory_->read16(addr);
        deref += static_cast<std::uint16_t>(register_y_);
        return deref;
    }
    default:
        log_error("Mode {} is not supproted.", static_cast<int>(mode));
        return 0u;
    }
}

void Cpu::op_and(AddressingMode mode)
{
    std::uint16_t addr = get_operand_address(mode);
    register_a_ &= memory_->read8(addr);
    update_zero_and_negative(register_a_);
}

void Cpu::op_cmp(AddressingMode mode, std::uint8_t compare_with)
{
    std::uint16_t addr = get_operand_address(mode);
    std::uint8_t value = memory_->read8(addr);

    set_carry(compare_with >= value);
    update_zero_and_negative(compare_with - value);
}

void Cpu::op_dec(AddressingMode mode)
{
    std::uint16_t addr = get_operand_address(mode);
    std::uint8_t value = memory_->read8(addr);
    value--;
    memory_->write8(addr, value);
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
    std::uint16_t addr = get_operand_address(mode);
    register_a_ ^= memory_->read8(addr);
    update_zero_and_negative(register_a_);
}

void Cpu::op_inc(AddressingMode mode)
{
    std::uint16_t addr = get_operand_address(mode);
    std::uint8_t value = memory_->read8(addr);
    value++;
    memory_->write8(addr, value);
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

void Cpu::op_lda(AddressingMode mode)
{
    std::uint16_t addr = get_operand_address(mode);
    register_a_ = memory_->read8(addr);
    update_zero_and_negative(register_a_);
}

void Cpu::op_ldx(AddressingMode mode)
{
    std::uint16_t addr = get_operand_address(mode);
    register_x_ = memory_->read8(addr);
    update_zero_and_negative(register_x_);
}

void Cpu::op_ldy(AddressingMode mode)
{
    std::uint16_t addr = get_operand_address(mode);
    register_y_ = memory_->read8(addr);
    update_zero_and_negative(register_y_);
}

void Cpu::op_lsr_accumulator()
{
    set_carry(register_a_ & 0x1);
    register_a_ >>= 1;
    update_zero_and_negative(register_a_);
}

void Cpu::op_lsr(AddressingMode mode)
{
    std::uint16_t addr = get_operand_address(mode);
    std::uint8_t value = memory_->read8(addr);
    set_carry(value & 0x1);
    value >>= 1;
    memory_->write8(addr, value);
    update_zero_and_negative(value);
}

void Cpu::op_ora(AddressingMode mode)
{
    std::uint16_t addr = get_operand_address(mode);
    register_a_ |= memory_->read8(addr);
    update_zero_and_negative(register_a_);
}

void Cpu::op_sta(AddressingMode mode)
{
    std::uint16_t addr = get_operand_address(mode);
    memory_->write8(addr, register_a_);
}

void Cpu::op_tax()
{
    register_x_ = register_a_;
    update_zero_and_negative(register_x_);
}
