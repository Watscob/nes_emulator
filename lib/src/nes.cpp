#include "nes.hpp"
#include "bus.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"

Nes::Nes()
    : Nes(nullptr)
{
}

Nes::Nes(std::function<void(Nes&)> callback)
    : cartridge_(std::make_shared<Cartridge>())
    , bus_(std::make_shared<Bus>(cartridge_))
    , cpu_(std::make_shared<Cpu>(bus_))
    , callback_(callback)
{
}

bool Nes::load_rom(std::vector<uint8_t> raw)
{
    return cartridge_->load_rom(raw);
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
