#ifndef BUS_HPP
#define BUS_HPP

#include <cstdint>
#include <memory>
#include <vector>

/*
 *  _______________ $10000  _______________
 * | PRG-ROM       |       |               |
 * | Upper Bank    |       |               |
 * |_ _ _ _ _ _ _ _| $C000 | PRG-ROM       |
 * | PRG-ROM       |       |               |
 * | Lower Bank    |       |               |
 * |_______________| $8000 |_______________|
 * | SRAM          |       | SRAM          |
 * |_______________| $6000 |_______________|
 * | Expansion ROM |       | Expansion ROM |
 * |_______________| $4020 |_______________|
 * | I/O Registers |       |               |
 * |_ _ _ _ _ _ _ _| $4000 |               |
 * | Mirrors       |       | I/O Registers |
 * | $2000-$2007   |       |               |
 * |_ _ _ _ _ _ _ _| $2008 |               |
 * | I/O Registers |       |               |
 * |_______________| $2000 |_______________|
 * | Mirrors       |       |               |
 * | $0000-$07FF   |       |               |
 * |_ _ _ _ _ _ _ _| $0800 |               |
 * | RAM           |       | RAM           |
 * |_ _ _ _ _ _ _ _| $0200 |               |
 * | Stack         |       |               |
 * |_ _ _ _ _ _ _ _| $0100 |               |
 * | Zero Page     |       |               |
 * |_______________| $0000 |_______________|
 */

class Cartridge;

class Bus
{
  public:
    Bus() = delete;
    explicit Bus(const std::shared_ptr<Cartridge>& cartridge);
    ~Bus() = default;

    void reset();

    uint8_t read8(uint16_t addr) const;
    void write8(uint16_t addr, uint8_t value);

    uint16_t read16(uint16_t addr) const;
    void write16(uint16_t addr, uint16_t value);

  private:
    static constexpr uint16_t RAM_SIZE      = 0x2000;
    static constexpr uint16_t RAM_START     = 0x0000;
    static constexpr uint16_t RAM_END       = 0x1FFF;
    static constexpr uint16_t PRG_ROM_START = 0x8000;
    static constexpr uint16_t PRG_ROM_END   = 0xFFFF;

    std::vector<uint8_t> ram_;
    std::shared_ptr<Cartridge> cartridge_;
};

#endif /* BUS_HPP */
