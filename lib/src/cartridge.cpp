#include "cartridge.hpp"
#include <fstream>
#include <iterator>
#include <stdexcept>
#include "log.hpp"

Cartridge::Cartridge(const std::string& path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file)
        throw std::runtime_error("Cannot open file " + path + ".");

    if (file.peek() == std::ifstream::traits_type::eof())
        throw std::runtime_error("Empty file " + path + ".");

    std::vector<uint8_t> header(HEADER_SIZE);
    if (!file.read(reinterpret_cast<char*>(header.data()), HEADER_SIZE))
        throw std::runtime_error("Failed to read ROM header.");

    if (!std::equal(NES_TAG.begin(), NES_TAG.end(), header.begin()))
        throw std::runtime_error("Invalid NES tag.");

    mapper_ = (header.at(7) & 0xF0) | (header.at(6) >> 4);

    uint8_t ines_version = (header.at(7) >> 2) & 0x3;
    if (ines_version != 0)
        throw std::runtime_error("NES2.0 format is not supported.");

    bool four_screen        = (header.at(6) & 0x8) != 0;
    bool vertical_mirroring = (header.at(6) & 0x1) != 0;

    if (four_screen)
        screen_mirroring_ = Mirroring::FOUR_SCREEN;
    else if (vertical_mirroring)
        screen_mirroring_ = Mirroring::VERTICAL;
    else
        screen_mirroring_ = Mirroring::HORIZONTAL;

    bool trainer = (header.at(6) & 0x4) != 0;
    if (trainer)
        throw std::runtime_error("Trainer is not supported.");

    uint8_t banks = header.at(4);
    if (!banks)
        throw std::runtime_error("ROM has no PRG-ROM banks.");

    uint8_t vbanks = header.at(5);

    prg_rom_.resize(banks * PRG_ROM_PAGE_SIZE);
    if (!file.read(reinterpret_cast<char*>(prg_rom_.data()), banks * PRG_ROM_PAGE_SIZE))
        throw std::runtime_error("Failed to read PRG-ROM from file.");

    if (vbanks)
    {
        chr_rom_.resize(vbanks * CHR_ROM_PAGE_SIZE);
        if (!file.read(reinterpret_cast<char*>(chr_rom_.data()), vbanks * CHR_ROM_PAGE_SIZE))
            throw std::runtime_error("Failed to read CHR-ROM from file.");
    }
}

Cartridge::Cartridge(std::vector<uint8_t> raw)
    : mapper_(0)
    , screen_mirroring_(Mirroring::VERTICAL)
{
    prg_rom_ = raw;
    prg_rom_.resize(PRG_ROM_PAGE_SIZE);
    prg_rom_.at(0x3FFC) = 0x00;
    prg_rom_.at(0x3FFD) = 0x80;
    // TODO BETTER, CREATE PSEUDO ROM FILE AND PARSE WITH THE OTHER CONSTRUCTOR
}

uint8_t Cartridge::read_prg(uint16_t addr)
{
    if (prg_rom_.size() == PRG_ROM_PAGE_SIZE)
        addr %= PRG_ROM_PAGE_SIZE;
    return prg_rom_.at(addr);
}

uint8_t Cartridge::read_chr(uint16_t addr)
{
    // TODO
    (void) addr;
    return 0u;
}
