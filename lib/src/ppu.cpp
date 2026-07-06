#include "ppu.hpp"
#include "cartridge.hpp"

Ppu::Ppu(const std::shared_ptr<Cartridge>& cartridge)
    : palette_table_(PALETTE_SIZE)
    , vram_(VRAM_SIZE)
    , oam_data_(OAM_SIZE)
    , cartridge_(cartridge)
{
}
