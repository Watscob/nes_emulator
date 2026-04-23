#include "cpu.hpp"
#include "log.hpp"

void Cpu::run(std::vector<std::uint8_t> program)
{
    program_counter_ = 0;

    while (1)
    {
        std::uint8_t code = program.at(program_counter_++);

        switch (code)
        {
        case 0x00: // BRK
            return;
        case 0xA9: // LDA (imm)
            register_a_ = program.at(program_counter_++);
            update_zero_and_negative(register_a_);
            break;
        case 0xAA: // TAX
            register_x_ = register_a_;
            update_zero_and_negative(register_x_);
            break;
        default:
            log_error("Code {:#04x} is not implemented.", code);
            break;
        }
    }
}
