#include <fstream>
#include <iostream>
#include <nes.hpp>
#include <string>

int main(int argc, char* argv[])
{
    Nes nes;

    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <rom_file>" << std::endl;
        return 1;
    }

    if (!nes.load_rom(std::string(argv[1])))
        return 1;

    nes.reset();

    while (nes.step()) {}

    return 0;
}
