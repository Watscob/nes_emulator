#include "nes.hpp"
#include <iostream>
#include <stdexcept>

Nes::Nes()
    : bus_(std::make_shared<NesBus>())
    , cpu_(std::make_shared<NesCpu>(bus_))
{
}

bool Nes::load_rom(const std::string& path)
{
    try
    {
        bus_->set_cartridge(std::make_shared<NesCartridge>(path));
        return true;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool Nes::load_rom(const std::vector<uint8_t>& rom)
{
    try
    {
        bus_->set_cartridge(std::make_shared<NesCartridge>(rom));
        return true;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

void Nes::reset()
{
    bus_->reset();
    cpu_->reset();
}

bool Nes::step()
{
    return cpu_->step();
}
