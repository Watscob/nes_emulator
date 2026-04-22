#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <vector>

class Cpu
{
  public:
    std::uint8_t register_a_;
    std::uint8_t status_;
    std::uint16_t program_counter_;

    Cpu()
        : register_a_(0u)
        , status_(0u)
        , program_counter_(0u)
    {
    }

    ~Cpu() = default;

    void run(std::vector<std::uint8_t> program);
};

#endif /* CPU_HPP */
