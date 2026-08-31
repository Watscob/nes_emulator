#include "nes_ppu_memory.hpp"

#include <stdexcept>

NesPpuMemory::NesPpuMemory()
    : cartridge_(nullptr)
    , oam_()
    , palette_()
    , vram_()
{
}

void NesPpuMemory::connect_cartridge(const std::shared_ptr<NesCartridge>& cartridge)
{
    cartridge_ = cartridge;
}

void NesPpuMemory::reset()
{
    oam_.fill(0u);
    palette_.fill(0u);
    vram_.fill(0u);
}

uint8_t NesPpuMemory::read(uint16_t addr) const
{
    switch (addr)
    {
    case CHR_ROM_START ... CHR_ROM_END:
        return cartridge_->read_chr(addr);
    case VRAM_MIRRORS_START ... VRAM_MIRRORS_END:
        return vram_.at(decode_nametable_(addr));
    case PALETTE_MIRRORS_START ... PALETTE_MIRRORS_END:
        return palette_.at(decode_palette_(addr));
    default:
        /* Ignoring memory read access */
        return 0u;
    }
}

void NesPpuMemory::write(uint16_t addr, uint8_t value)
{
    switch (addr)
    {
    case CHR_ROM_START ... CHR_ROM_END:
        /* Ignoring write access to CHR ROM */
        break;
    case VRAM_MIRRORS_START ... VRAM_MIRRORS_END:
        vram_.at(decode_nametable_(addr)) = value;
        break;
    case PALETTE_MIRRORS_START ... PALETTE_MIRRORS_END:
        palette_.at(decode_palette_(addr)) = value;
        break;
    default:
        /* Ignoring memory write access */
        break;
    }
}

uint8_t NesPpuMemory::oam_read(uint8_t addr) const
{
    return oam_.at(addr);
}

void NesPpuMemory::oam_write(uint8_t addr, uint8_t value)
{
    oam_.at(addr) = value;
}

uint16_t NesPpuMemory::get_vram_base_addr() const
{
    return VRAM_MIRRORS_START;
}

uint16_t NesPpuMemory::get_palette_base_addr() const
{
    return PALETTE_MIRRORS_START;
}

size_t NesPpuMemory::get_oam_size() const
{
    return OAM_SIZE;
}

uint16_t NesPpuMemory::decode_nametable_(uint16_t addr) const
{
    uint16_t mirrored_vram = addr & 0x2FFF;
    uint16_t vram_index    = mirrored_vram - VRAM_MIRRORS_START;
    uint16_t name_table    = vram_index / 0x400;

    Mirroring mirroring = cartridge_ ? cartridge_->get_mirroring() : Mirroring::VERTICAL;

    if (mirroring == Mirroring::VERTICAL && (name_table == 2u || name_table == 3u))
        return vram_index - 0x800;
    else if (mirroring == Mirroring::HORIZONTAL && (name_table == 1u || name_table == 2u))
        return vram_index - 0x400;
    else if (mirroring == Mirroring::HORIZONTAL && name_table == 3u)
        return vram_index - 0x800;
    else
        return vram_index;
}

uint16_t NesPpuMemory::decode_palette_(uint16_t addr) const
{
    uint16_t offset = (addr - PALETTE_MIRRORS_START) & 0x001F;
    if ((offset & 0x0013) == 0x0010)
        offset &= 0x000F;
    return offset;
}
