#include "nes_bus.hpp"
#include <format>
#include <stdexcept>

NesBus::NesBus()
    : ram_()
    , cartridge_(nullptr)
    , joypad_1_(nullptr)
    , ppu_(nullptr)
    , cycles_(0u)
{
}

void NesBus::connect_cartridge(const std::shared_ptr<NesCartridge>& cartridge)
{
    cartridge_ = cartridge;
}

void NesBus::connect_joypad_1(const std::shared_ptr<NesJoypad>& joypad)
{
    joypad_1_ = joypad;
}

void NesBus::connect_ppu(const std::shared_ptr<NesPpu>& ppu)
{
    ppu_ = ppu;
}

void NesBus::reset()
{
    ram_.fill(0u);
    cycles_ = 0u;
}

void NesBus::step(uint8_t cycles)
{
    cycles_ += cycles;
    ppu_->step(cycles);
}

uint8_t NesBus::read8(uint16_t addr) const
{
    switch (addr)
    {
    case RAM_MIRRORS_START ... RAM_MIRRORS_END:
        return ram_.at(addr & RAM_MIRRORS_MASK);
    case PPU_REG_MIRRORS_START ... PPU_REG_MIRRORS_END:
        return ppu_->read(addr & PPU_MIRRORS_MASK);
    case APU_REG_START ... APU_REG_END:
        return 0u;
    case PPU_OAM_DMA:
        throw std::runtime_error(std::format("Attempt to read PPU OAM DMA (0x{:#04x}).", addr));
        break;
    case APU_REG_STATUS_CTRL:
        return 0u;
    case JOYPAD_1:
        return joypad_1_->read();
    case JOYPAD_2:
        return 0u;
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
    case RAM_MIRRORS_START ... RAM_MIRRORS_END:
        ram_.at(addr & RAM_MIRRORS_MASK) = value;
        break;
    case PPU_REG_MIRRORS_START ... PPU_REG_MIRRORS_END:
        ppu_->write(addr & PPU_MIRRORS_MASK, value);
        break;
    case APU_REG_START ... APU_REG_END:
        break;
    case PPU_OAM_DMA:
        for (uint16_t offset = 0u; offset < 256u; offset++)
            ppu_->write_oam_dma(read8((static_cast<uint16_t>(value) << 8u) | offset));
        break;
    case APU_REG_STATUS_CTRL:
        break;
    case JOYPAD_1:
        joypad_1_->write(value);
        break;
    case JOYPAD_2:
        break;
    case PRG_ROM_START ... PRG_ROM_END:
        throw std::runtime_error(std::format("Attempt to write to PRG ROM space ({:#04x}).", addr));
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

bool NesBus::poll_nmi()
{
    return ppu_->poll_nmi();
}
