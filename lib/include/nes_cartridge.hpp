#ifndef NES_CARTRIDGE_HPP
#define NES_CARTRIDGE_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

enum class Mirroring
{
    VERTICAL,
    HORIZONTAL,
    FOUR_SCREEN
};

class NesCartridge
{
  public:
    NesCartridge() = delete;
    explicit NesCartridge(const std::string& path);
    explicit NesCartridge(const std::vector<uint8_t>& raw);
    ~NesCartridge() = default;

    uint8_t read_prg(uint16_t addr) const;
    uint8_t read_chr(uint16_t addr) const;

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

    void init(const std::vector<uint8_t>& raw);
};

#endif /* NES_CARTRIDGE_HPP */
