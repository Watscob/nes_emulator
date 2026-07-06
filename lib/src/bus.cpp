#include "bus.hpp"
#include <stdexcept>
#include "cartridge.hpp"
#include "log.hpp"
#include "ppu.hpp"

Bus::Bus(const std::shared_ptr<Cartridge>& cartridge, const std::shared_ptr<Ppu>& ppu)
    : ram_(RAM_SIZE)
    , cartridge_(cartridge)
    , ppu_(ppu)
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
    case 0x2000:
    case 0x2001:
    case 0x2003:
    case 0x2005:
    case 0x2006:
    case 0x4014:
        throw std::runtime_error("Attempt to read to write-only PPU address.");
    case 0x2002:
        return ppu_->read_status();
    case 0x2004:
        return ppu_->read_oam_data();
    case 0x2007:
        return ppu_->read_data();
    case 0x2008 ... PPU_REGISTERS_END:
        return read8(addr & 0x2007);
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
    case 0x2000:
        ppu_->write_ctrl(value);
        break;
    case 0x2001:
        ppu_->write_mask(value);
        break;
    case 0x2002:
        throw std::runtime_error("Attempt to write to PPU status register.");
    case 0x2003:
        ppu_->write_oam_addr(value);
        break;
    case 0x2004:
        ppu_->write_oam_data(value);
        break;
    case 0x2005:
        ppu_->write_scroll(value);
        break;
    case 0x2006:
        ppu_->write_ppu_addr(value);
        break;
    case 0x2007:
        ppu_->write_data(value);
        break;
    case 0x2008 ... PPU_REGISTERS_END:
        write8(addr & 0x2007, value);
        break;
    case PRG_ROM_START ... PRG_ROM_END:
        throw std::runtime_error("Attempt to write to PRG ROM space.");
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
