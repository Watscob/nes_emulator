#include "nes_cpu.hpp"

NesCpu::NesCpu(const std::shared_ptr<NesBus>& bus)
    : ra_(0u)
    , rx_(0u)
    , ry_(0u)
    , sp_(STACK_RESET)
    , pc_(0x8000)
    , status_()
    , bus_(bus)
{
}

void NesCpu::load_rom(const std::vector<uint8_t>& rom)
{
    (void) rom;
    pc_ = 0u;
}

bool NesCpu::step()
{
    return false;
}
