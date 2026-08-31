#ifndef NES_PPU_REGISTERS_HPP
#define NES_PPU_REGISTERS_HPP

#include <cstdint>
#include "nes_registers.hpp"

class NesPpuRegisters
{
  public:
    NesPpuCtrl ctrl;
    NesPpuMask mask;
    NesPpuStatus status;
    uint8_t oam_addr   = 0u;
    uint16_t vram_addr = 0u;
    uint16_t tmp_addr  = 0u;
    uint8_t fine_x     = 0u;
    bool addr_latch    = false;
};

#endif /* NES_PPU_REGISTERS_HPP */
