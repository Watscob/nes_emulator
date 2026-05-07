#include "bus.hpp"
#include <stdexcept>
#include "cartridge.hpp"
#include "log.hpp"

Bus::Bus(const std::shared_ptr<Cartridge>& cartridge)
    : ram_(RAM_SIZE)
    , cartridge_(cartridge)
{
}

void Bus::reset()
{
    ram_.assign(ram_.size(), 0u);
}

uint8_t Bus::read8(uint16_t addr) const
{
    switch (addr)
    {
    case RAM_START ... RAM_END:
        return ram_.at(addr - RAM_START);
    case PRG_ROM_START ... PRG_ROM_END:
        return cartridge_->read_prg(addr - PRG_ROM_START);
    default:
        log_error("Ignoring memory read access at {:#06x}.", addr);
        return 0u;
    }
}

void Bus::write8(uint16_t addr, uint8_t value)
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
        log_error("Ignoring memory write access at {:#06x}.", addr);
        break;
    }
}

uint16_t Bus::read16(uint16_t addr) const
{
    uint16_t lo = read8(addr);
    uint16_t hi = read8(addr + 1);
    return (hi << 8u) | lo;
}

void Bus::write16(uint16_t addr, uint16_t value)
{
    uint8_t hi = static_cast<uint8_t>(value >> 8);
    uint8_t lo = static_cast<uint8_t>(value & 0xFF);
    write8(addr, lo);
    write8(addr + 1, hi);
}
