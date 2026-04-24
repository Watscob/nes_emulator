#include <cstdint>
#include <vector>
#include "cpu.hpp"
#include "log.hpp"

int main()
{
    set_log_level(Logger::LogLevel::DEBUG);

    std::vector<std::uint8_t> rom = {0x00};

    Cpu nes;
    nes.load_and_run(rom);

    return 0;
}
