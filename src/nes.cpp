#include "nes.hpp"
#include "cpu.hpp"
#include "memory.hpp"

Nes::Nes()
    : Nes(nullptr)
{
}

Nes::Nes(std::function<void(Nes&)> callback)
    : memory_(std::make_shared<Memory>())
    , cpu_(std::make_shared<Cpu>(memory_))
    , callback_(callback)
{
}

void Nes::load_and_run(std::vector<uint8_t> rom, uint16_t start_addr)
{
    load(rom, start_addr);
    reset();
    while (step()) {}
}

void Nes::load(std::vector<uint8_t> rom, uint16_t start_addr)
{
    memory_->load(start_addr, rom);
    memory_->write16(0xFFFC, start_addr);
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
