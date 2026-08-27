#ifndef NES_HPP
#define NES_HPP

#include <memory>
#include <string>
#include <vector>
#include "nes_bus.hpp"
#include "nes_cpu.hpp"

class Nes
{
  public:
    explicit Nes();
    ~Nes() = default;

    bool load_rom(const std::string& path);
    bool load_rom(const std::vector<uint8_t>& rom);
    void reset();
    bool step();

  private:
    std::shared_ptr<NesBus> bus_;
    std::shared_ptr<NesCpu> cpu_;
};

#endif /* NES_HPP */
