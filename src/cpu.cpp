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
            // TODO
        default:
            log_error("Code {:#04x} is not implemented.", code);
            break;
        }
    }
}
