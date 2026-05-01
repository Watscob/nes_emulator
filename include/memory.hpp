#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <array>
#include <cstdint>
#include <vector>

class Memory
{
  public:
    Memory()
        : memory_()
    {
    }

    ~Memory() = default;

    uint8_t read8(uint16_t addr);
    void write8(uint16_t addr, uint8_t value);

    uint16_t read16(uint16_t addr);
    void write16(uint16_t addr, uint16_t value);

    void load(uint16_t addr, std::vector<uint8_t> mem);

  private:
    static constexpr uint32_t RAM_SIZE = 0x10000;

    std::array<uint8_t, RAM_SIZE> memory_;
};

#endif /* MEMORY_HPP */
