#ifndef NES_BUS_HPP
#define NES_BUS_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include "nes_cartridge.hpp"

class NesBus
{
  public:
    explicit NesBus();
    ~NesBus() = default;

    void set_cartridge(const std::shared_ptr<NesCartridge>& cartridge);
    void reset();

    uint8_t read8(uint16_t addr) const;
    uint16_t read16(uint16_t addr) const;

    void write8(uint16_t addr, uint8_t value);
    void write16(uint16_t addr, uint16_t value);

  private:
    static constexpr uint16_t RAM_SIZE      = 0x2000;
    static constexpr uint16_t RAM_START     = 0x0000;
    static constexpr uint16_t RAM_END       = 0x1FFF;
    static constexpr uint16_t PRG_ROM_START = 0x8000;
    static constexpr uint16_t PRG_ROM_END   = 0xFFFF;

    std::vector<uint8_t> ram_;
    std::shared_ptr<NesCartridge> cartridge_;
};

#endif /* NES_BUS_HPP */
