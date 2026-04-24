#include "cpu.hpp"
#include "log.hpp"

void Cpu::load_and_run(std::vector<std::uint8_t> rom)
{
    load(rom);
    run();
}

void Cpu::load(std::vector<std::uint8_t> rom)
{
    memory_->load(0x8000, rom);
    program_counter_ = 0x8000;
}

void Cpu::run()
{
    while (1)
    {
        std::uint8_t code = memory_->read8(program_counter_++);

        switch (code)
        {
        case 0x00: // BRK
            return;
        case 0xA9: // LDA (imm)
            register_a_ = memory_->read8(program_counter_++);
            update_zero_and_negative(register_a_);
            break;
        case 0xAA: // TAX
            register_x_ = register_a_;
            update_zero_and_negative(register_x_);
            break;
        case 0xE8: // INX
            register_x_++;
            update_zero_and_negative(register_x_);
            break;
        default:
            log_error("Code {:#04x} is not implemented.", code);
            break;
        }
    }
}
