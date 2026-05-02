#include "bus.hpp"
#include <stdexcept>
#include "log.hpp"

Bus::Bus()
    : ram_(RAM_SIZE)
{
}

uint8_t Bus::read8(uint16_t addr)
{
    try
    {
        return ram_.at(addr);
    }
    catch (const std::out_of_range&)
    {
        log_error("Ignoring memory read access at {:#06x}.", addr);
        return 0u;
    }
}

void Bus::write8(uint16_t addr, uint8_t value)
{
    try
    {
        ram_.at(addr) = value;
    }
    catch (const std::out_of_range&)
    {
        log_error("Ignoring memory write access at {:#06x}.", addr);
    }
}

uint16_t Bus::read16(uint16_t addr)
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

void Bus::load(uint16_t addr, std::vector<uint8_t> data)
{
    if (addr + data.size() > ram_.size())
    {
        log_error("Cannot load data of size {} at addr {}.", data.size(), addr);
        return;
    }

    std::copy(data.begin(), data.end(), ram_.begin() + addr);
}
