#include "memory.hpp"
#include <stdexcept>
#include "log.hpp"

std::uint8_t Memory::read8(std::uint16_t addr)
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

void Memory::write8(std::uint16_t addr, std::uint8_t value)
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

std::uint16_t Memory::read16(std::uint16_t addr)
{
    std::uint16_t lo = static_cast<std::uint16_t>(read8(addr));
    std::uint16_t hi = static_cast<std::uint16_t>(read8(addr + 1));
    return (hi << 8u) | lo;
}

void Memory::write16(std::uint16_t addr, std::uint16_t value)
{
    std::uint8_t hi = static_cast<std::uint8_t>(value >> 8);
    std::uint8_t lo = static_cast<std::uint8_t>(value & 0xFF);
    write8(addr, lo);
    write8(addr + 1, hi);
}

void Memory::load(std::uint16_t addr, std::vector<std::uint8_t> data)
{
    if (addr + data.size() > memory_.size())
    {
        log_error("Cannot load data of size {} at addr {}.", data.size(), addr);
        return;
    }

    std::copy(data.begin(), data.end(), memory_.begin() + addr);
}
