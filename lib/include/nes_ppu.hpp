#ifndef NES_PPU_HPP
#define NES_PPU_HPP

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include "nes_ppu_memory.hpp"
#include "nes_ppu_registers.hpp"

class NesCartridge;

class NesPpu
{
  private:
    static constexpr size_t FRAME_WIDTH  = 256u;
    static constexpr size_t FRAME_HEIGHT = 240u;

    static constexpr uint16_t PPUCTRL   = 0x0000;
    static constexpr uint16_t PPUMASK   = 0x0001;
    static constexpr uint16_t PPUSTATUS = 0x0002;
    static constexpr uint16_t OAMADDR   = 0x0003;
    static constexpr uint16_t OAMDATA   = 0x0004;
    static constexpr uint16_t PPUSCROLL = 0x0005;
    static constexpr uint16_t PPUADDR   = 0x0006;
    static constexpr uint16_t PPUDATA   = 0x0007;

    static constexpr uint16_t SCANLINES_PER_FRAME    = 262u;
    static constexpr uint8_t PPU_TICKS_PER_CPU_TICK  = 3u;
    static constexpr uint16_t PPU_TICKS_PER_SCANLINE = 341u;
    static constexpr uint8_t VBLANK_START_SCANLINE   = 241u;

    static const std::array<uint32_t, 64u> SYSTEM_PALETTE;

  public:
    explicit NesPpu();
    ~NesPpu() = default;

    void connect_cartridge(const std::shared_ptr<NesCartridge>& cartridge);

    void reset();
    void step(uint8_t cycles);

    size_t get_frame_width() const;
    size_t get_frame_height() const;
    const std::array<uint32_t, FRAME_WIDTH * FRAME_HEIGHT>& get_frame() const;

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t value);

    void write_oam_dma(uint8_t value);

    bool poll_nmi();
    bool poll_frame_ready();

  private:
    std::array<uint32_t, FRAME_WIDTH * FRAME_HEIGHT> frame_;

    NesPpuMemory memory_;
    NesPpuRegisters registers_;

    uint8_t open_bus_;
    uint8_t data_buf_;

    uint16_t scanline_;
    size_t cycles_;

    bool nmi_pending_;
    bool frame_ready_;

    std::function<void(const std::vector<uint32_t>&, size_t)> render_callback_;

    const std::array<uint8_t, 4u> get_bg_palette_(size_t tile_col, size_t tile_row) const;
    const std::array<uint8_t, 4u> get_sprite_palette_(uint8_t palette_i) const;
    void set_pixel_(size_t x, size_t y, uint32_t color);
    void render_frame_();
};

#endif /* NES_PPU_HPP */
