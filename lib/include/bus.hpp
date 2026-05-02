#ifndef BUS_HPP
#define BUS_HPP

#include <cstdint>
#include <vector>

class Bus
{
  public:
    Bus();
    ~Bus() = default;

    uint8_t read8(uint16_t addr);
    void write8(uint16_t addr, uint8_t value);

    uint16_t read16(uint16_t addr);
    void write16(uint16_t addr, uint16_t value);

    void load(uint16_t addr, std::vector<uint8_t> data);

  private:
    static constexpr uint32_t RAM_SIZE = 0x10000;

    std::vector<uint8_t> ram_;
};

#endif /* BUS_HPP */
