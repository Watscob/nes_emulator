#include "nes.hpp"

Nes::Nes()
    : bus_(std::make_shared<NesBus>())
    , cpu_(std::make_shared<NesCpu>(bus_))
{
}

void Nes::load_rom(const std::vector<uint8_t>& rom)
{
    bus_->reset();
    bus_->set_cartridge(std::make_shared<NesCartridge>(rom));
    cpu_->reset();
}

bool Nes::step()
{
    return cpu_->step();
}
