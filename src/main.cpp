#include <format>
#include <fstream>
#include <iostream>
#include <nes.hpp>
#include <string>

int main(int argc, char* argv[])
{
    Nes nes;

    if (argc < 2)
    {
        std::cout << std::format("Usage: {} <rom_file>", argv[0]) << std::endl;
        return 1;
    }

    nes.load_rom(std::string(argv[1]));
    nes.reset();
    nes.run();

    return 0;
}
