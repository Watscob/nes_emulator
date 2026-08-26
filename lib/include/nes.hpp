#ifndef NES_HPP
#define NES_HPP

#include <memory>
#include <vector>
#include "nes_cpu.hpp"

class Nes
{
  public:
    explicit Nes();
    ~Nes() = default;

    void load_rom(const std::vector<uint8_t>& rom);
    bool step();

  private:
    std::shared_ptr<NesCpu> cpu_;
};

#endif /* NES_HPP */
