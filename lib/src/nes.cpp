#include "nes.hpp"

Nes::Nes()
    : cpu_(std::make_shared<NesCpu>())
{
}

void Nes::load_rom(const std::vector<uint8_t>& rom)
{
    cpu_->load_rom(rom);
}

bool Nes::step()
{
    return cpu_->step();
}
