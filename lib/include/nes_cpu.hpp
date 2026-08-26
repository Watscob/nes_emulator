#ifndef NES_CPU_HPP
#define NES_CPU_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include "nes_bus.hpp"
#include "nes_registers.hpp"

class NesCpu
{
  public:
    NesCpu() = delete;
    explicit NesCpu(const std::shared_ptr<NesBus>& bus);
    ~NesCpu() = default;

    void load_rom(const std::vector<uint8_t>& rom);
    bool step();

  private:
    static constexpr uint16_t STACK_BASE = 0x0100;
    static constexpr uint8_t STACK_RESET = 0xFD;

    uint8_t ra_;
    uint8_t rx_;
    uint8_t ry_;
    uint8_t sp_;
    uint16_t pc_;
    NesCpuStatus status_;
    std::shared_ptr<NesBus> bus_;
};

#endif /* NES_CPU_HPP */
