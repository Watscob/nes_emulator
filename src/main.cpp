#include <bus.hpp>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <fstream>
#include <functional>
#include <iterator>
#include <log.hpp>
#include <memory>
#include <nes.hpp>
#include <vector>
#include "sdl.hpp"

#if __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>
#endif

constexpr int SCREEN_WIDTH  = 32;
constexpr int SCREEN_HEIGHT = 32;

static bool running = false;
static bool quit    = false;

static std::unique_ptr<Sdl> sdl;
static std::unique_ptr<Nes> nes;
static std::vector<uint32_t> frame;

static bool handle_input(Nes& nes);
static bool read_screen_state(Nes& nes);
static void sdl_callback(Nes& nes);
static bool init_emulator();
static void start_emulator();
static void start_emulator();
static bool load_rom_from_file(const std::string& path);
#if __EMSCRIPTEN__
static void emscripten_loop_wrapper();
#endif

#if __EMSCRIPTEN__
EM_JS(void, sdl_init_canvas, (size_t width, size_t height), {
    const canvas         = document.getElementById('canvas');
    canvas.width         = width;
    canvas.height        = height;
    canvas.style.width   = width + 'px';
    canvas.style.height  = height + 'px';
    canvas.style.display = 'block';

    // const loading         = document.getElementById('loading');
    // loading.style.display = 'none';
});
#endif

static bool handle_input(Nes& nes)
{
    std::vector<Sdl::Key> inputs = sdl->get_inputs();

    for (auto& input : inputs)
    {
        switch (input)
        {
        case Sdl::Key::QUIT:
            return false;
        case Sdl::Key::W:
            nes.get_bus()->write8(0xFF, 0x77);
            break;
        case Sdl::Key::S:
            nes.get_bus()->write8(0xFF, 0x73);
            break;
        case Sdl::Key::A:
            nes.get_bus()->write8(0xFF, 0x61);
            break;
        case Sdl::Key::D:
            nes.get_bus()->write8(0xFF, 0x64);
            break;
        default:
            break;
        }
    }

    return true;
}

static bool read_screen_state(Nes& nes)
{
    bool update = false;

    for (uint16_t i = 0x0200; i < 0x0600; i++)
    {
        uint8_t color_idx = nes.get_bus()->read8(i);
        uint32_t rgb      = sdl->get_color(color_idx);

        if (frame.at(i - 0x0200) != rgb)
        {
            frame.at(i - 0x0200) = rgb;
            update               = true;
        }
    }

    return update;
}

static void sdl_callback(Nes& nes)
{
    if (!handle_input(nes))
        quit = true;

    nes.get_bus()->write8(0x00FE, std::rand() % 16 + 1);

    if (read_screen_state(nes))
        sdl->render(frame);
}

static bool init_emulator()
{
    set_log_level(Logger::LogLevel::DEBUG);
    log_info("NES emulator initialization ...");

    std::srand(std::time({}));

    if (!sdl)
    {
        sdl = std::make_unique<Sdl>(SCREEN_WIDTH, SCREEN_HEIGHT);
        frame.resize(SCREEN_WIDTH * SCREEN_HEIGHT);

        std::function<void(int, int)> cb;
#if __EMSCRIPTEN__
        cb = sdl_init_canvas;
#endif

        if (!sdl->init(cb))
            return false;
    }

    if (!nes)
        nes = std::make_unique<Nes>(sdl_callback);

    return true;
}

static void start_emulator()
{
    if (!sdl || !nes)
    {
        log_error("NES emulator is not initialized.");
        return;
    }

    if (running)
    {
        log_error("NES emulator is already running.");
        return;
    }

    quit    = false;
    running = true;

    log_info("NES emulator running ...");

#if __EMSCRIPTEN__
    emscripten_set_main_loop(&emscripten_loop_wrapper, 0, false);
#else
    while (!quit && nes->step()) {}
#endif
}

static void stop_emulator()
{
#if __EMSCRIPTEN__
    emscripten_cancel_main_loop();
#endif
    quit    = true;
    running = false;

    frame.assign(frame.size(), 0u);
    sdl->render(frame);
}

static bool load_rom_from_file(const std::string& path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file || !file.is_open())
    {
        log_error("Cannot open file {}.", path);
        return false;
    }

    std::vector<uint8_t> rom((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

    if (!nes || !nes->load_rom(rom))
        return false;

    nes->reset();
    return true;
}

#if __EMSCRIPTEN__
static void emscripten_loop_wrapper()
{
    if (quit)
    {
        stop_emulator();
        return;
    }

    for (int i = 0; i < 100; i++)
    {
        if (!nes->step())
        {
            quit = true;
            break;
        }
    }
}

EMSCRIPTEN_BINDINGS(Wrappers)
{
    emscripten::function("init_emulator", &init_emulator);
    emscripten::function("start_emulator", &start_emulator);
    emscripten::function("stop_emulator", &stop_emulator);
    emscripten::function("load_rom_from_file", &load_rom_from_file);
};
#else
int main(int argc, char* argv[])
{
    if (!init_emulator())
        return 1;

    if (argc < 2)
    {
        log_error("Usage: {} <rom_file>", argv[0]);
        return 1;
    }

    if (!load_rom_from_file(std::string(argv[1])))
        return 1;

    start_emulator();

    log_info("NES emulator exiting ...");
    return 0;
}
#endif
