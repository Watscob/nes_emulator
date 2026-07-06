#include "cartridge.hpp"
#include "log.hpp"

bool Cartridge::load_rom(const std::vector<uint8_t>& raw)
{
    if (!std::equal(NES_TAG.begin(), NES_TAG.end(), raw.begin()))
    {
        log_error("Invalid NES tag.");
        return false;
    }

    mapper_ = (raw.at(7) & 0xF0) | (raw.at(6) >> 4);

    uint8_t ines_version = (raw.at(7) >> 2) & 0x3;
    if (ines_version != 0)
    {
        log_error("NES2.0 format is not supported.");
        return false;
    }

    bool four_screen        = (raw.at(6) & 0x8) != 0;
    bool vertical_mirroring = (raw.at(6) & 0x1) != 0;

    if (four_screen)
        screen_mirroring_ = Mirroring::FOUR_SCREEN;
    else if (vertical_mirroring)
        screen_mirroring_ = Mirroring::VERTICAL;
    else
        screen_mirroring_ = Mirroring::HORIZONTAL;

    bool trainer = (raw.at(6) & 0x4) != 0;
    if (trainer)
    {
        log_error("Trainer is not supported.");
        return false;
    }

    uint8_t banks = raw.at(4);
    if (!banks)
    {
        log_error("ROM has no PRG-ROM banks.");
        return false;
    }

    uint8_t vbanks = raw.at(5);

    uint16_t prg_size   = banks * PRG_ROM_PAGE_SIZE;
    uint16_t chr_size   = vbanks * CHR_ROM_PAGE_SIZE;
    uint16_t total_size = HEADER_SIZE + prg_size + chr_size;

    if (total_size > raw.size())
    {
        log_error("Total size of the ROM does not match header.");
        return false;
    }

    prg_rom_.assign(raw.begin() + HEADER_SIZE, raw.begin() + HEADER_SIZE + prg_size);

    if (vbanks)
    {
        chr_rom_.assign(raw.begin() + HEADER_SIZE + prg_size,
                        raw.begin() + HEADER_SIZE + prg_size + chr_size);
    }

    return true;
}

uint8_t Cartridge::read_prg(uint16_t addr) const
{
    if (prg_rom_.size() == PRG_ROM_PAGE_SIZE)
        addr %= PRG_ROM_PAGE_SIZE;
    return prg_rom_.at(addr);
}

uint8_t Cartridge::read_chr(uint16_t addr) const
{
    return chr_rom_.at(addr);
}
