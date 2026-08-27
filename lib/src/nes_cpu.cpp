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

void NesCpu::stack_push8(uint8_t value)
{
    bus_->write8(STACK_BASE + sp_, value);
    sp_--;
}

void NesCpu::stack_push16(uint16_t value)
{
    stack_push8(static_cast<uint8_t>(value >> 8u));
    stack_push8(static_cast<uint8_t>(value & 0xFF));
}

uint8_t NesCpu::stack_pop8()
{
    sp_++;
    return bus_->read8(STACK_BASE + sp_);
}

uint16_t NesCpu::stack_pop16()
{
    uint16_t lo = stack_pop8();
    uint16_t hi = stack_pop8();
    return (hi << 8u) | lo;
}
