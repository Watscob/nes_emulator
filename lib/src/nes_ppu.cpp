#include "nes_ppu.hpp"
#include <ranges>
#include <stdexcept>

const std::array<uint32_t, 64u> NesPpu::SYSTEM_PALETTE = {
    {0x808080FF, 0x003DA6FF, 0x0012B0FF, 0x440096FF, 0xA1005EFF, 0xC70028FF, 0xBA0600FF,
     0x8C1700FF, 0x5C2F00FF, 0x104500FF, 0x054A00FF, 0x00472EFF, 0x004166FF, 0x000000FF,
     0x050505FF, 0x050505FF, 0xC7C7C7FF, 0x0077FFFF, 0x2155FFFF, 0x8237FAFF, 0xEB2FB5FF,
     0xFF2950FF, 0xFF2200FF, 0xD63200FF, 0xC46200FF, 0x358000FF, 0x058F00FF, 0x008A55FF,
     0x0099CCFF, 0x212121FF, 0x090909FF, 0x090909FF, 0xFFFFFFFF, 0x0FD7FFFF, 0x69A2FFFF,
     0xD480FFFF, 0xFF45F3FF, 0xFF618BFF, 0xFF8833FF, 0xFF9C12FF, 0xFABC20FF, 0x9FE30EFF,
     0x2BF035FF, 0x0CF0A4FF, 0x05FBFFFF, 0x5E5E5EFF, 0x0D0D0DFF, 0x0D0D0DFF, 0xFFFFFFFF,
     0xA6FCFFFF, 0xB3ECFFFF, 0xDAABEBFF, 0xFFA8F9FF, 0xFFABB3FF, 0xFFD2B0FF, 0xFFEFA6FF,
     0xFFF79CFF, 0xD7E895FF, 0xA6EDAFFF, 0xA2F2DAFF, 0x99FFFCFF, 0xDDDDDDFF, 0x111111FF,
     0x111111FF}};

NesPpu::NesPpu()
    : frame_()
    , memory_()
    , registers_()
    , open_bus_(0u)
    , data_buf_(0u)
    , scanline_(0u)
    , cycles_(0u)
    , nmi_pending_(false)
    , frame_ready_(false)
{
}

void NesPpu::connect_cartridge(const std::shared_ptr<NesCartridge>& cartridge)
{
    memory_.connect_cartridge(cartridge);
}

void NesPpu::reset()
{
    frame_.fill(0u);
    memory_.reset();
    registers_   = NesPpuRegisters();
    data_buf_    = 0u;
    scanline_    = 0u;
    cycles_      = 0u;
    nmi_pending_ = false;
    frame_ready_ = false;
}

void NesPpu::step(uint8_t cycles)
{
    cycles_ += (cycles * PPU_TICKS_PER_CPU_TICK);

    if (cycles_ >= PPU_TICKS_PER_SCANLINE)
    {
        cycles_ -= PPU_TICKS_PER_SCANLINE;
        scanline_++;

        if (scanline_ == VBLANK_START_SCANLINE)
        {
            registers_.status.vblank(1u);
            registers_.status.sprite_zero_hit(0u);
            if (registers_.ctrl.nmi_enable())
                nmi_pending_ = true;
        }

        if (scanline_ >= SCANLINES_PER_FRAME)
        {
            scanline_    = 0u;
            nmi_pending_ = false;
            registers_.status.sprite_zero_hit(0u);
            registers_.status.vblank(0u);
            render_frame_();
        }
    }
}

size_t NesPpu::get_frame_width() const
{
    return FRAME_WIDTH;
}

size_t NesPpu::get_frame_height() const
{
    return FRAME_HEIGHT;
}

const std::array<uint32_t, NesPpu::FRAME_WIDTH * NesPpu::FRAME_HEIGHT>& NesPpu::get_frame() const
{
    return frame_;
}

uint8_t NesPpu::read(uint16_t addr)
{
    uint8_t result = open_bus_;

    switch (addr)
    {
    case PPUSTATUS:
        result = registers_.status.get();
        registers_.status.vblank(0u);
        registers_.addr_latch = false;
        break;
    case OAMDATA:
        result = memory_.oam_read(registers_.oam_addr);
        break;
    case PPUDATA:
        result    = data_buf_;
        data_buf_ = memory_.read(registers_.vram_addr);
        if (registers_.vram_addr >= memory_.get_palette_base_addr())
            result = data_buf_;
        registers_.vram_addr += registers_.ctrl.increment() ? 32u : 1u;
        break;
    default:
        break;
    }

    open_bus_ = result;
    return result;
}

void NesPpu::write(uint16_t addr, uint8_t value)
{
    NesPpuCtrl value_reg;

    switch (addr)
    {
    case PPUCTRL:
    {
        value_reg.set(value);
        if (!registers_.ctrl.nmi_enable() && value_reg.nmi_enable() && registers_.status.vblank())
            nmi_pending_ = true;
        registers_.ctrl.set(value);
        registers_.tmp_addr =
            (registers_.tmp_addr & 0xF3FF) | ((static_cast<uint16_t>(value) & 0x03) << 10u);
        break;
    }
    case PPUMASK:
        registers_.mask.set(value);
        break;
    case OAMADDR:
        registers_.oam_addr = value;
        break;
    case OAMDATA:
        memory_.oam_write(registers_.oam_addr, value);
        registers_.oam_addr++;
        break;
    case PPUSCROLL:
        if (!registers_.addr_latch)
        {
            registers_.fine_x   = value & 0x07;
            registers_.tmp_addr = (registers_.tmp_addr & 0xFFE0) | (value >> 3);
        }
        else
        {
            registers_.tmp_addr =
                (registers_.tmp_addr & 0x8FFF) | ((static_cast<uint16_t>(value) & 0x07) << 12u);
            registers_.tmp_addr =
                (registers_.tmp_addr & 0xFC1F) | ((static_cast<uint16_t>(value) & 0xF8) << 2u);
        }
        registers_.addr_latch = !registers_.addr_latch;
        break;
    case PPUADDR:
        if (!registers_.addr_latch)
        {
            registers_.tmp_addr =
                (registers_.tmp_addr & 0x80FF) | ((static_cast<uint16_t>(value) & 0x3F) << 8u);
        }
        else
        {
            registers_.tmp_addr  = (registers_.tmp_addr & 0xFF00) | value;
            registers_.vram_addr = registers_.tmp_addr;
        }
        registers_.addr_latch = !registers_.addr_latch;
        break;
    case PPUDATA:
        memory_.write(registers_.vram_addr, value);
        registers_.vram_addr += registers_.ctrl.increment() ? 32u : 1u;
        break;
    default:
        break;
    }
}

void NesPpu::write_oam_dma(uint8_t value)
{
    write(OAMDATA, value);
}

bool NesPpu::poll_nmi()
{
    bool tmp     = nmi_pending_;
    nmi_pending_ = false;
    return tmp;
}

bool NesPpu::poll_frame_ready()
{
    bool tmp     = frame_ready_;
    frame_ready_ = false;
    return tmp;
}

const std::array<uint8_t, 4u> NesPpu::get_bg_palette_(size_t tile_x, size_t tile_y) const
{
    uint16_t attr_table_i = tile_y / 4u * 8u + tile_x / 4u;
    uint8_t attr_byte     = memory_.read(memory_.get_vram_base_addr() + 0x03C0 + attr_table_i);

    size_t quadrant_x = (tile_x % 4u) / 2u;
    size_t quadrant_y = (tile_y % 4u) / 2u;

    size_t quadrant   = quadrant_y * 2u + quadrant_x;
    uint8_t palette_i = (attr_byte >> (quadrant * 2u)) & 0x3;

    uint16_t palette_start = 1u + palette_i * 4u;
    return {memory_.read(memory_.get_palette_base_addr()),
            memory_.read(memory_.get_palette_base_addr() + palette_start),
            memory_.read(memory_.get_palette_base_addr() + palette_start + 1u),
            memory_.read(memory_.get_palette_base_addr() + palette_start + 2u)};
}

const std::array<uint8_t, 4u> NesPpu::get_sprite_palette_(uint8_t palette_i) const
{
    uint16_t palette_start = 0x11 + (palette_i * 4u);
    return {0u,
            memory_.read(memory_.get_palette_base_addr() + palette_start),
            memory_.read(memory_.get_palette_base_addr() + palette_start + 1u),
            memory_.read(memory_.get_palette_base_addr() + palette_start + 2u)};
}

void NesPpu::set_pixel_(size_t x, size_t y, uint32_t color)
{
    if (x < FRAME_WIDTH && y < FRAME_HEIGHT)
        frame_.at(y * FRAME_WIDTH + x) = color;
}

void NesPpu::render_frame_()
{
    if (registers_.mask.show_bg())
    {
        uint16_t bank = static_cast<uint16_t>(registers_.ctrl.background()) * 0x1000u;

        for (size_t i : std::views::iota(0, 0x03C0))
        {
            size_t tile_n                         = memory_.read(memory_.get_vram_base_addr() + i);
            size_t tile_x                         = i % 32u;
            size_t tile_y                         = i / 32u;
            const std::array<uint8_t, 4u> palette = get_bg_palette_(tile_x, tile_y);

            for (size_t y : std::views::iota(0u, 8u))
            {
                uint8_t upper = memory_.read(bank + tile_n * 16u + y);
                uint8_t lower = memory_.read(bank + tile_n * 16u + y + 8u);

                for (size_t x : std::views::iota(0u, 8u) | std::views::reverse)
                {
                    size_t pix_x = tile_x * 8u + x;
                    size_t pix_y = tile_y * 8u + y;

                    if (pix_x < 8u && !registers_.mask.show_bg_left8())
                        continue;

                    uint8_t value = (1u & lower) << 1u | (1u & upper);
                    upper >>= 1u;
                    lower >>= 1u;

                    uint32_t color = SYSTEM_PALETTE.at(palette.at(value));
                    set_pixel_(pix_x, pix_y, color);
                }
            }
        }
    }

    if (registers_.mask.show_sprites())
    {
        uint16_t base;

        for (size_t i : std::views::iota(0u, memory_.get_oam_size()) | std::views::stride(4u) |
                            std::views::reverse)
        {
            size_t tile_n                         = memory_.oam_read(i + 1u);
            size_t tile_x                         = memory_.oam_read(i + 3u);
            size_t tile_y                         = memory_.oam_read(i);
            uint8_t tile_a                        = memory_.oam_read(i + 2u);
            bool flip_vertical                    = (tile_a >> 7u) & 0x1;
            bool flip_horizontal                  = (tile_a >> 6u) & 0x1;
            const std::array<uint8_t, 4u> palette = get_sprite_palette_(tile_a & 0x3);
            size_t sprite_height                  = registers_.ctrl.sprite_height() ? 16u : 8u;

            for (size_t y : std::views::iota(0u, sprite_height))
            {
                if (sprite_height == 16u)
                {
                    uint8_t tile  = tile_n & 0xFE;
                    uint16_t bank = (tile_n & 0x1) * 0x1000;
                    uint8_t row   = (y >= 8u) ? 1u : 0u;
                    base          = bank + (tile + row) * 16u + (y & 0x7);
                }
                else
                {
                    base = registers_.ctrl.sprite_table() * 0x1000 + tile_n * 16u + y;
                }

                uint8_t upper = memory_.read(base);
                uint8_t lower = memory_.read(base + 8u);

                for (size_t x : std::views::iota(0u, 8u) | std::views::reverse)
                {
                    size_t pix_x = flip_horizontal ? (tile_x + 7u - x) : (tile_x + x);
                    size_t pix_y =
                        flip_vertical ? (tile_y + (sprite_height - 1u) - y) : (tile_y + y);

                    if (pix_x < 8u && !registers_.mask.show_sprites_left8())
                        continue;

                    uint8_t value = (1u & lower) << 1u | (1u & upper);
                    upper >>= 1u;
                    lower >>= 1u;

                    if (value == 0u)
                        continue;

                    uint32_t color = SYSTEM_PALETTE.at(palette.at(value));
                    set_pixel_(pix_x, pix_y, color);
                }
            }
        }
    }

    frame_ready_ = true;
}
