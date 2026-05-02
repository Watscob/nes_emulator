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
    Nes() = delete;
    explicit Nes(const std::string& path);
    explicit Nes(std::function<void(Nes&)> callback, const std::string& path);
    explicit Nes(std::vector<uint8_t> raw);
    explicit Nes(std::function<void(Nes&)> callback, std::vector<uint8_t> raw);
    ~Nes() = default;

    void reset();
    bool step();

    std::shared_ptr<Bus> get_bus() { return bus_; }
    std::shared_ptr<Cpu> get_cpu() { return cpu_; }

  private:
    std::shared_ptr<Cartridge> cartridge_;
    std::shared_ptr<Bus> bus_;
    std::shared_ptr<Cpu> cpu_;
    std::function<void(Nes&)> callback_;
};

#endif /* NES_HPP */
