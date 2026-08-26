#include "nes_bus.hpp"
#include <iostream>
#include <stdexcept>

NesBus::NesBus()
    : ram_(RAM_SIZE)
{
}

void NesBus::set_cartridge(const std::shared_ptr<NesCartridge>& cartridge)
{
    cartridge_ = cartridge;
}

void NesBus::reset()
{
    ram_.assign(ram_.size(), 0u);
}

uint8_t NesBus::read8(uint16_t addr) const
{
    switch (addr)
    {
    case RAM_START ... RAM_END:
        return ram_.at(addr - RAM_START);
    case PRG_ROM_START ... PRG_ROM_END:
        return cartridge_ ? cartridge_->read_prg(addr - PRG_ROM_START) : 0u;
    default:
        /* Ignoring memory read access */
        return 0u;
    }
}

uint16_t NesBus::read16(uint16_t addr) const
{
    uint16_t lo = read8(addr);
    uint16_t hi = read8(addr + 1u);
    return (hi << 8u) | lo;
}

void NesBus::write8(uint16_t addr, uint8_t value)
{
    switch (addr)
    {
    case RAM_START ... RAM_END:
        ram_.at(addr - RAM_START) = value;
        break;
    case PRG_ROM_START ... PRG_ROM_END:
        throw std::runtime_error("Attempt to write to PRG ROM space.");
        break;
    default:
        /* Ignoring memory write access */
        break;
    }
}

void NesBus::write16(uint16_t addr, uint16_t value)
{
    uint8_t hi = static_cast<uint8_t>(value >> 8u);
    uint8_t lo = static_cast<uint8_t>(value & 0xFF);
    write8(addr, lo);
    write8(addr + 1u, hi);
}
