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

void NesCpu::reset()
{
    ra_     = 0u;
    rx_     = 0u;
    ry_     = 0u;
    status_ = NesCpuStatus();
    sp_     = STACK_RESET;

    pc_ = bus_->read16(0xFFFC);
}

bool NesCpu::step()
{
    return false;
}
