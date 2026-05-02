#include "nes.hpp"
#include "bus.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"

Nes::Nes(const std::string& path)
    : Nes(nullptr, path)
{
}

Nes::Nes(std::function<void(Nes&)> callback, const std::string& path)
    : cartridge_(std::make_shared<Cartridge>(path))
    , bus_(std::make_shared<Bus>(cartridge_))
    , cpu_(std::make_shared<Cpu>(bus_))
    , callback_(callback)
{
}

Nes::Nes(std::vector<uint8_t> raw)
    : Nes(nullptr, raw)
{
}

Nes::Nes(std::function<void(Nes&)> callback, std::vector<uint8_t> raw)
    : cartridge_(std::make_shared<Cartridge>(raw))
    , bus_(std::make_shared<Bus>(cartridge_))
    , cpu_(std::make_shared<Cpu>(bus_))
    , callback_(callback)
{
}

void Nes::reset()
{
    cpu_->reset();
}

bool Nes::step()
{
    if (callback_)
        callback_(*this);

    return cpu_->step();
}
