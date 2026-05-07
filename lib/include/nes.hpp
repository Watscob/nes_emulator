#ifndef NES_HPP
#define NES_HPP

#include <functional>
#include <memory>
#include <vector>

class Cartridge;
class Cpu;
class Bus;

class Nes
{
  public:
    explicit Nes();
    explicit Nes(std::function<void(Nes&)> callback);
    ~Nes() = default;

    bool load_rom(const std::vector<uint8_t>& raw);
    void reset();
    bool step();

    const std::shared_ptr<Bus>& get_bus() const { return bus_; }
    const std::shared_ptr<Cpu>& get_cpu() const { return cpu_; }

  private:
    std::shared_ptr<Cartridge> cartridge_;
    std::shared_ptr<Bus> bus_;
    std::shared_ptr<Cpu> cpu_;
    std::function<void(Nes&)> callback_;
};

#endif /* NES_HPP */
