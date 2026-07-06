#include "ppu.hpp"
#include <stdexcept>
#include "cartridge.hpp"
#include "log.hpp"

Ppu::Ppu(const std::shared_ptr<Cartridge>& cartridge)
    : addr_()
    , ctrl_()
    , mask_()
    , scroll_()
    , status_()
    , oam_addr_(0u)
    , internal_data_buf_(0u)
    , palette_table_(PALETTE_SIZE)
    , vram_(VRAM_SIZE)
    , oam_data_(OAM_SIZE)
    , cartridge_(cartridge)
{
}

uint8_t Ppu::read_data()
{
    uint8_t result = 0u;
    uint16_t addr  = static_cast<uint16_t>(addr_);
    increment_vram_addr();

    switch (addr)
    {
    case 0 ... 0x1FFF:
        result             = internal_data_buf_;
        internal_data_buf_ = cartridge_->read_chr(addr);
        break;
    case 0x2000 ... 0x2FFF:
        result             = internal_data_buf_;
        internal_data_buf_ = vram_.at(mirror_vram_addr(addr));
        break;
    case 0x3000 ... 0x3EFF:
        log_error("Ignoring PPU read access at {:#06x}.", addr);
        break;
    case 0x3F00 ... 0x3FFF:
        result = palette_table_[addr - 0x3F00];
        break;
    default:
        log_error("Ignoring PPU read access at {:#06x}.", addr);
        break;
    }

    return result;
}

uint8_t Ppu::read_oam_data()
{
    return oam_data_.at(oam_addr_);
}

uint8_t Ppu::read_status()
{
    uint8_t status = static_cast<uint8_t>(status_);
    status_.reset_vblank_status();
    addr_.reset_latch();
    scroll_.reset_latch();
    return status;
}

void Ppu::write_ctrl(uint8_t value)
{
    ctrl_.set(value);
}

void Ppu::write_data(uint8_t value)
{
    uint16_t addr = static_cast<uint16_t>(addr_);

    switch (addr)
    {
    case 0 ... 0x1FFF:
        throw std::runtime_error("Attempt to write to CHR ROM space.");
    case 0x2000 ... 0x2FFF:
        vram_.at(mirror_vram_addr(addr)) = value;
        break;
    case 0x3000 ... 0x3EFF:
        log_error("Ignoring PPU write access at {:#06x}.", addr);
        break;
    case 0x3F00 ... 0x3FFF:
        if (addr == 0x3F10 || addr == 0x3F14 || addr == 0x3F18 || addr == 0x3F1C)
            addr -= 0x10;
        palette_table_.at(addr - 0x3F00) = value;
        break;
    default:
        log_error("Ignoring PPU write access at {:#06x}.", addr);
        break;
    }

    increment_vram_addr();
}

void Ppu::write_mask(uint8_t value)
{
    mask_.set(value);
}

void Ppu::write_oam_addr(uint8_t value)
{
    oam_addr_ = value;
}
void Ppu::write_oam_data(uint8_t value)
{
    oam_data_.at(oam_addr_) = value;
    oam_addr_++;
}

void Ppu::write_ppu_addr(uint8_t value)
{
    addr_.set(value);
}

void Ppu::write_scroll(uint8_t value)
{
    scroll_.set(value);
}

void Ppu::increment_vram_addr()
{
    addr_ += ctrl_.get_vram_addr_increment();
}

uint16_t Ppu::mirror_vram_addr(uint16_t addr)
{
    uint16_t mirrored_vram = addr & 0x2FFF;
    uint16_t vram_index    = mirrored_vram - 0x2000;
    uint16_t name_table    = vram_index / 0x400;

    if (cartridge_->get_mirroring() == Mirroring::VERTICAL &&
        (name_table == 2u || name_table == 3u))
        return vram_index - 0x800;
    else if (cartridge_->get_mirroring() == Mirroring::HORIZONTAL &&
             (name_table == 1u || name_table == 2u))
        return vram_index - 0x400;
    else if (cartridge_->get_mirroring() == Mirroring::HORIZONTAL && name_table == 3u)
        return vram_index - 0x800;
    else
        return vram_index;
}
