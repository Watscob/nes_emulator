#ifndef PPU_HPP
#define PPU_HPP

#include <bitset>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

class Cartridge;

class Ppu
{
  public:
    Ppu() = delete;
    explicit Ppu(const std::shared_ptr<Cartridge>& cartridge);
    ~Ppu() = default;

    uint8_t read_data();
    uint8_t read_oam_data();
    uint8_t read_status();

    void write_ctrl(uint8_t value);
    void write_data(uint8_t value);
    void write_mask(uint8_t value);
    void write_oam_addr(uint8_t value);
    void write_oam_data(uint8_t value);
    void write_ppu_addr(uint8_t value);
    void write_scroll(uint8_t value);

  private:
    class AddrRegister
    {
      public:
        constexpr explicit AddrRegister()
            : value_(0u, 0u)
            , hi_ptr_(true)
        {
        }
        ~AddrRegister() = default;

        constexpr operator uint16_t() const
        {
            return (static_cast<uint16_t>(value_.first) << 8) | value_.second;
        }

        constexpr void set(uint8_t value)
        {
            if (hi_ptr_)
                value_.first = value;
            else
                value_.second = value;

            set_pair(static_cast<uint16_t>(*this) & 0x3FFF);
            hi_ptr_ = !hi_ptr_;
        }

        constexpr AddrRegister& operator+=(uint8_t inc)
        {
            uint8_t lo = value_.second;
            value_.second += inc;
            if (lo > value_.second)
                value_.first++;
            set_pair(static_cast<uint16_t>(*this) & 0x3FFF);
            return *this;
        }

        constexpr void reset_latch() { hi_ptr_ = true; }

      private:
        std::pair<uint8_t, uint8_t> value_;
        bool hi_ptr_;

        constexpr void set_pair(uint16_t value)
        {
            value_.first  = static_cast<uint8_t>(value >> 8);
            value_.second = static_cast<uint8_t>(value & 0xFF);
        }
    };

    class ControlRegister
    {
      public:
        constexpr explicit ControlRegister()
            : bits_(0u)
        {
        }
        ~ControlRegister() = default;

        constexpr void set(uint8_t value) { bits_ = value; }

        constexpr uint16_t get_nametable_addr() const
        {
            switch (static_cast<uint8_t>(bits_.to_ulong()) & 0x3)
            {
            case 0u:
                return 0x2000;
            case 1u:
                return 0x2400;
            case 2u:
                return 0x2800;
            default:
                return 0x2C00;
            }
        }

        constexpr uint8_t get_vram_addr_increment() const
        {
            return bits_.test(VRAM_ADDR_INCREMENT_POS) ? 32u : 1u;
        }
        constexpr uint16_t get_sprite_pattern_addr() const
        {
            return bits_.test(SPRITE_PATTERN_ADDR_POS) ? 0x1000 : 0u;
        }
        constexpr uint16_t get_background_pattern_addr() const
        {
            return bits_.test(BACKGROUND_PATTERN_POS) ? 0x1000 : 0u;
        }
        constexpr uint8_t get_sprite_size() const { return bits_.test(SPRITE_SIZE_POS) ? 16u : 8u; }
        constexpr bool get_master_slave_select() const
        {
            return bits_.test(MASTER_SLAVE_SELECT_POS);
        }
        constexpr bool generate_vblank_nmi() const { return bits_.test(GENERATE_NMI_POS); }

      private:
        static constexpr uint8_t NAMETABLE1_POS          = 0;
        static constexpr uint8_t NAMETABLE2_POS          = 1;
        static constexpr uint8_t VRAM_ADDR_INCREMENT_POS = 2;
        static constexpr uint8_t SPRITE_PATTERN_ADDR_POS = 3;
        static constexpr uint8_t BACKGROUND_PATTERN_POS  = 4;
        static constexpr uint8_t SPRITE_SIZE_POS         = 5;
        static constexpr uint8_t MASTER_SLAVE_SELECT_POS = 6;
        static constexpr uint8_t GENERATE_NMI_POS        = 7;

        std::bitset<8> bits_;
    };

    class MaskRegister
    {
      public:
        constexpr explicit MaskRegister()
            : bits_(0u)
        {
        }
        ~MaskRegister() = default;

        constexpr void set(uint8_t value) { bits_ = value; }

        constexpr bool is_greyscale() const { return bits_.test(GREYSCALE_POS); }
        constexpr bool get_leftmost_8pxl_background() const
        {
            return bits_.test(LEFTMOST_8PXL_BACKGROUND);
        }
        constexpr bool get_leftmost_8pxl_sprite() const { return bits_.test(LEFTMOST_8PXL_SPRITE); }
        constexpr bool show_background() const { return bits_.test(SHOW_BACKGROUND); }
        constexpr bool show_sprites() const { return bits_.test(SHOW_SPRITES); }

        constexpr std::tuple<bool, bool, bool> emphasise()
        {
            return std::tuple<bool, bool, bool>(bits_.test(EMPHASISE_RED),
                                                bits_.test(EMPHASISE_BLUE),
                                                bits_.test(EMPHASISE_GREEN));
        }

      private:
        static constexpr uint8_t GREYSCALE_POS            = 0;
        static constexpr uint8_t LEFTMOST_8PXL_BACKGROUND = 1;
        static constexpr uint8_t LEFTMOST_8PXL_SPRITE     = 2;
        static constexpr uint8_t SHOW_BACKGROUND          = 3;
        static constexpr uint8_t SHOW_SPRITES             = 4;
        static constexpr uint8_t EMPHASISE_RED            = 5;
        static constexpr uint8_t EMPHASISE_GREEN          = 6;
        static constexpr uint8_t EMPHASISE_BLUE           = 7;

        std::bitset<8> bits_;
    };

    class ScrollRegister
    {
      public:
        constexpr explicit ScrollRegister()
            : scroll_x_(0u)
            , scroll_y_(0u)
            , latch_(false)
        {
        }
        ~ScrollRegister() = default;

        constexpr void set(uint8_t value)
        {
            if (latch_)
                scroll_y_ = value;
            else
                scroll_x_ = value;
            latch_ = !latch_;
        }

        constexpr void reset_latch() { latch_ = false; }

      private:
        uint8_t scroll_x_;
        uint8_t scroll_y_;
        bool latch_;
    };

    class StatusRegister
    {
      public:
        constexpr explicit StatusRegister()
            : bits_(0u)
        {
        }
        ~StatusRegister() = default;

        constexpr operator uint8_t() const { return static_cast<uint8_t>(bits_.to_ulong()); }

        constexpr bool is_in_vblank() { return bits_.test(VBLANK_STARTED_POS); }
        constexpr void reset_vblank_status() { bits_.reset(VBLANK_STARTED_POS); }

        constexpr void set_vblank_status(bool status) { bits_.set(VBLANK_STARTED_POS, status); }
        constexpr void set_sprite_zero_hit(bool status) { bits_.set(SPRITE_ZERO_HIT_POS, status); }
        constexpr void set_sprite_overflow(bool status) { bits_.set(SPRITE_OVERFLOW_POS, status); }

      private:
        static constexpr uint8_t NOT_USED_POS        = 0;
        static constexpr uint8_t NOT_USED2_POS       = 1;
        static constexpr uint8_t NOT_USED3_POS       = 2;
        static constexpr uint8_t NOT_USED4_POS       = 3;
        static constexpr uint8_t NOT_USED5_POS       = 4;
        static constexpr uint8_t SPRITE_OVERFLOW_POS = 5;
        static constexpr uint8_t SPRITE_ZERO_HIT_POS = 6;
        static constexpr uint8_t VBLANK_STARTED_POS  = 7;

        std::bitset<8> bits_;
    };

    static constexpr uint16_t PALETTE_SIZE = 0x20;
    static constexpr uint16_t VRAM_SIZE    = 0x800;
    static constexpr uint16_t OAM_SIZE     = 0x100;

    AddrRegister addr_;
    ControlRegister ctrl_;
    MaskRegister mask_;
    ScrollRegister scroll_;
    StatusRegister status_;
    uint8_t oam_addr_;
    uint8_t internal_data_buf_;
    std::vector<uint8_t> palette_table_;
    std::vector<uint8_t> vram_;
    std::vector<uint8_t> oam_data_;
    std::shared_ptr<Cartridge> cartridge_;

    void increment_vram_addr();
    uint16_t mirror_vram_addr(uint16_t addr);
};

#endif /* PPU_HPP */
