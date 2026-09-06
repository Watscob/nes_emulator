#ifndef NES_BUS_HPP
#define NES_BUS_HPP

#include <array>
#include <cstdint>
#include <memory>
#include "nes_cartridge.hpp"
#include "nes_joypad.hpp"
#include "nes_ppu.hpp"

class NesBus
{
  private:
    static constexpr size_t RAM_SIZE                = 2048u;
    static constexpr uint16_t RAM_MIRRORS_START     = 0x0000;
    static constexpr uint16_t RAM_MIRRORS_END       = 0x1FFF;
    static constexpr uint16_t PPU_REG_MIRRORS_START = 0x2000;
    static constexpr uint16_t PPU_REG_MIRRORS_END   = 0x3FFF;
    static constexpr uint16_t APU_REG_START         = 0x4000;
    static constexpr uint16_t APU_REG_END           = 0x4013;
    static constexpr uint16_t PPU_OAM_DMA           = 0x4014;
    static constexpr uint16_t APU_REG_STATUS_CTRL   = 0x4015;
    static constexpr uint16_t JOYPAD_1              = 0x4016;
    static constexpr uint16_t JOYPAD_2              = 0x4017;
    static constexpr uint16_t PRG_ROM_START         = 0x8000;
    static constexpr uint16_t PRG_ROM_END           = 0xFFFF;

    static constexpr uint16_t RAM_MIRRORS_MASK = 0x07FF;
    static constexpr uint16_t PPU_MIRRORS_MASK = 0x0007;

  public:
    explicit NesBus();
    ~NesBus() = default;

    void connect_cartridge(const std::shared_ptr<NesCartridge>& cartridge);
    void connect_joypad_1(const std::shared_ptr<NesJoypad>& joypad);
    void connect_ppu(const std::shared_ptr<NesPpu>& ppu);

    void reset();
    void step(uint8_t cycles);

    uint8_t read8(uint16_t addr) const;
    uint16_t read16(uint16_t addr) const;

    void write8(uint16_t addr, uint8_t value);
    void write16(uint16_t addr, uint16_t value);

    bool poll_nmi();

  private:
    std::array<uint8_t, RAM_SIZE> ram_;
    std::shared_ptr<NesCartridge> cartridge_;
    std::shared_ptr<NesJoypad> joypad_1_;
    std::shared_ptr<NesPpu> ppu_;

    size_t cycles_;
};

#endif /* NES_BUS_HPP */
