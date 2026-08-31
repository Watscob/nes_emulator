#ifndef NES_PPU_MEMORY_HPP
#define NES_PPU_MEMORY_HPP

#include <array>
#include <memory>
#include "nes_cartridge.hpp"

class NesPpuMemory
{
  private:
    static constexpr size_t OAM_SIZE = 256u;

    static constexpr uint16_t CHR_ROM_START = 0x0000;
    static constexpr uint16_t CHR_ROM_END   = 0x1FFF;

    static constexpr size_t VRAM_SIZE            = 2048u;
    static constexpr uint16_t VRAM_MIRRORS_START = 0x2000;
    static constexpr uint16_t VRAM_MIRRORS_END   = 0x3EFF;

    static constexpr size_t PALETTE_SIZE            = 32u;
    static constexpr uint16_t PALETTE_MIRRORS_START = 0x3F00;
    static constexpr uint16_t PALETTE_MIRRORS_END   = 0x3FFF;

  public:
    explicit NesPpuMemory();
    ~NesPpuMemory() = default;

    void connect_cartridge(const std::shared_ptr<NesCartridge>& cartridge);

    void reset();

    uint8_t read(uint16_t addr) const;
    void write(uint16_t addr, uint8_t value);

    uint8_t oam_read(uint8_t addr) const;
    void oam_write(uint8_t addr, uint8_t value);

    uint16_t get_vram_base_addr() const;
    uint16_t get_palette_base_addr() const;
    size_t get_oam_size() const;

  private:
    std::shared_ptr<NesCartridge> cartridge_;
    std::array<uint8_t, OAM_SIZE> oam_;
    std::array<uint8_t, PALETTE_SIZE> palette_;
    std::array<uint8_t, VRAM_SIZE> vram_;

    uint16_t decode_nametable_(uint16_t addr) const;
    uint16_t decode_palette_(uint16_t addr) const;
};

#endif /* NES_PPU_MEMORY_HPP */
