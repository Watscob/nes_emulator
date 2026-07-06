#ifndef PPU_HPP
#define PPU_HPP

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

class Cartridge;

class Ppu
{
  public:
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

            uint16_t addr = static_cast<uint16_t>(*this);
            if (addr > 0x3FFF)
              set_pair(addr & 0x3FFF);

            hi_ptr_ = !hi_ptr_;
        }

        constexpr void reset_latch()
        {
            hi_ptr_ = true;
        }

      private:
        std::pair<uint8_t, uint8_t> value_;
        bool hi_ptr_;

        constexpr void set_pair(uint16_t value)
        {
            value_.first = static_cast<uint8_t>(value >> 8);
            value_.second = static_cast<uint8_t>(value & 0xFF);
        }
    };

    Ppu() = delete;
    explicit Ppu(const std::shared_ptr<Cartridge>& cartridge);
    ~Ppu() = default;

  private:
    static constexpr uint16_t PALETTE_SIZE = 0x20;
    static constexpr uint16_t VRAM_SIZE    = 0x800;
    static constexpr uint16_t OAM_SIZE     = 0x100;

    std::vector<uint8_t> palette_table_;
    std::vector<uint8_t> vram_;
    std::vector<uint8_t> oam_data_;
    std::shared_ptr<Cartridge> cartridge_;
};

#endif /* PPU_HPP */
