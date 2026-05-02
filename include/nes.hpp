#ifndef NES_HPP
#define NES_HPP

#include <functional>
#include <memory>
#include <vector>

class Cpu;
class Memory;

class Nes
{
  public:
    Nes();
    explicit Nes(std::function<void(Nes&)> callback);
    ~Nes() = default;

    void load_and_run(std::vector<uint8_t> rom, uint16_t start_addr = 0x8000);
    void load(std::vector<uint8_t> rom, uint16_t start_addr = 0x8000);
    void reset();
    bool step();

    std::shared_ptr<Memory> get_memory() { return memory_; }
    std::shared_ptr<Cpu> get_cpu() { return cpu_; }

  private:
    std::shared_ptr<Memory> memory_;
    std::shared_ptr<Cpu> cpu_;
    std::function<void(Nes&)> callback_;
};

#endif /* NES_HPP */
