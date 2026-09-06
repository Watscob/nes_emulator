#include "nes_cartridge.hpp"
#include <fstream>
#include <stdexcept>

NesCartridge::NesCartridge(const std::string& path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file || !file.is_open())
        throw std::runtime_error("Cannot open file " + path + ".");

    std::vector<uint8_t> rom((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

    init_(rom);
}

NesCartridge::NesCartridge(const std::vector<uint8_t>& raw)
{
    init_(raw);
}

uint8_t NesCartridge::read_prg(uint16_t addr) const
{
    if (prg_rom_.size() == PRG_ROM_PAGE_SIZE)
        addr %= PRG_ROM_PAGE_SIZE;
    return prg_rom_.at(addr);
}

uint8_t NesCartridge::read_chr(uint16_t addr) const
{
    return chr_rom_.at(addr);
}

const Mirroring& NesCartridge::get_mirroring() const
{
    return screen_mirroring_;
}

void NesCartridge::init_(const std::vector<uint8_t>& raw)
{
    if (!std::equal(NES_TAG.begin(), NES_TAG.end(), raw.begin()))
        throw std::runtime_error("Invalid NES tag.");

    mapper_ = (raw.at(7u) & 0xF0) | (raw.at(6u) >> 4u);
    if (mapper_ != 0u)
        throw std::runtime_error("Only mapper 0 is supported.");

    uint8_t ines_version = (raw.at(7u) >> 2u) & 0x3;
    if (ines_version != 0u)
        throw std::runtime_error("NES2.0 format is not supported.");

    bool four_screen        = (raw.at(6u) & 0x8) != 0u;
    bool vertical_mirroring = (raw.at(6u) & 0x1) != 0u;

    if (four_screen)
        screen_mirroring_ = Mirroring::FOUR_SCREEN;
    else if (vertical_mirroring)
        screen_mirroring_ = Mirroring::VERTICAL;
    else
        screen_mirroring_ = Mirroring::HORIZONTAL;

    bool trainer = (raw.at(6u) & 0x4) != 0u;
    if (trainer)
        throw std::runtime_error("Trainer is not supported.");

    uint8_t banks = raw.at(4u);
    if (!banks)
        throw std::runtime_error("ROM has no PRG-ROM banks.");

    uint8_t vbanks = raw.at(5u);

    uint16_t prg_size   = banks * PRG_ROM_PAGE_SIZE;
    uint16_t chr_size   = vbanks * CHR_ROM_PAGE_SIZE;
    uint16_t total_size = HEADER_SIZE + prg_size + chr_size;

    if (total_size > raw.size())
        throw std::runtime_error("Total size of the ROM does not match header.");

    prg_rom_.assign(raw.begin() + HEADER_SIZE, raw.begin() + HEADER_SIZE + prg_size);

    if (vbanks)
    {
        chr_rom_.assign(raw.begin() + HEADER_SIZE + prg_size,
                        raw.begin() + HEADER_SIZE + prg_size + chr_size);
    }
}
