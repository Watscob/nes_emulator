#include "cpu.hpp"
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
        OpCode opcode = OPCODES.at(code);

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
        /* INX */
        case 0xE8:
            op_inx();
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

void Cpu::op_inx()
{
    register_x_++;
    update_zero_and_negative(register_x_);
}

void Cpu::op_lda(AddressingMode mode)
{
    std::uint16_t addr = get_operand_address(mode);
    register_a_ = memory_->read8(addr);
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
