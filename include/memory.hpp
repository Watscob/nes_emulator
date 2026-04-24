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

    std::uint8_t read8(std::uint16_t addr);
    void write8(std::uint16_t addr, std::uint8_t value);

    std::uint16_t read16(std::uint16_t addr);
    void write16(std::uint16_t addr, std::uint16_t value);

    void load(std::uint16_t addr, std::vector<std::uint8_t> mem);

  private:
    static constexpr std::uint32_t RAM_SIZE = 0x10000;

    std::array<std::uint8_t, RAM_SIZE> memory_;
};

#endif /* MEMORY_HPP */
