#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

#include <array>
#include <cstdint>
#include <string_view>
#include <vector>

enum class Mirroring
{
    VERTICAL,
    HORIZONTAL,
    FOUR_SCREEN
};

class Cartridge
{
  public:
    explicit Cartridge() {}
    ~Cartridge() = default;

    bool load_rom(const std::vector<uint8_t>& raw);

    uint8_t read_prg(uint16_t addr) const;
    uint8_t read_chr(uint16_t addr) const;

    const Mirroring& get_mirroring() const { return screen_mirroring_; }

  private:
    static constexpr std::string_view NES_TAG   = "NES\x1A";
    static constexpr uint16_t PRG_ROM_PAGE_SIZE = 0x4000;
    static constexpr uint16_t CHR_ROM_PAGE_SIZE = 0x2000;
    static constexpr uint16_t HEADER_SIZE       = 0x10;
    static constexpr uint16_t TRAINER_SIZE      = 0x200;

    std::vector<uint8_t> prg_rom_;
    std::vector<uint8_t> chr_rom_;
    uint8_t mapper_;
    Mirroring screen_mirroring_;
};

#endif /* CARTRIDGE_HPP */
