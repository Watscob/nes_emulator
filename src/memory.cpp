#include "memory.hpp"
#include <stdexcept>
#include "log.hpp"

uint8_t Memory::read8(uint16_t addr)
{
    try
    {
        return memory_.at(addr);
    }
    catch (const std::out_of_range&)
    {
        log_error("Ignoring memory read access at {:#06x}.", addr);
        return 0u;
    }
}

void Memory::write8(uint16_t addr, uint8_t value)
{
    try
    {
        memory_.at(addr) = value;
    }
    catch (const std::out_of_range&)
    {
        log_error("Ignoring memory write access at {:#06x}.", addr);
    }
}

uint16_t Memory::read16(uint16_t addr)
{
    uint16_t lo = read8(addr);
    uint16_t hi = read8(addr + 1);
    return (hi << 8u) | lo;
}

void Memory::write16(uint16_t addr, uint16_t value)
{
    uint8_t hi = static_cast<uint8_t>(value >> 8);
    uint8_t lo = static_cast<uint8_t>(value & 0xFF);
    write8(addr, lo);
    write8(addr + 1, hi);
}

void Memory::load(uint16_t addr, std::vector<uint8_t> data)
{
    if (addr + data.size() > memory_.size())
    {
        log_error("Cannot load data of size {} at addr {}.", data.size(), addr);
        return;
    }

    std::copy(data.begin(), data.end(), memory_.begin() + addr);
}
