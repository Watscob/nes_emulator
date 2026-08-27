#include "nes.hpp"
#include <ctime>
#include <iostream>
#include <stdexcept>

Nes::Nes(size_t screen_scale)
    : screen_(DEFAULT_SCREEN_WIDTH * DEFAULT_SCREEN_HEIGHT, 0u)
    , bus_(std::make_shared<NesBus>())
    , cpu_(std::make_shared<NesCpu>(bus_))
{
    std::srand(std::time({}));

    if (!SDL_Init(SDL_INIT_VIDEO))
        throw std::runtime_error("Fail to init SDL.");

    sdl_window_.reset(SDL_CreateWindow("NES Emulator",
                                       DEFAULT_SCREEN_WIDTH * screen_scale,
                                       DEFAULT_SCREEN_HEIGHT * screen_scale,
                                       SDL_WINDOW_RESIZABLE));

    if (!sdl_window_)
        throw std::runtime_error("Fail to create a window.");

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, sdl_window_.get());
    SDL_SetNumberProperty(props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, 1);
    sdl_renderer_.reset(SDL_CreateRendererWithProperties(props));

    if (!sdl_renderer_)
        throw std::runtime_error("Failed to create a render.");

    if (!SDL_SetRenderScale(sdl_renderer_.get(), screen_scale, screen_scale))
        throw std::runtime_error("Failed to scale renderer.");

    sdl_texture_.reset(SDL_CreateTexture(sdl_renderer_.get(),
                                         SDL_PIXELFORMAT_RGBA32,
                                         SDL_TEXTUREACCESS_STREAMING,
                                         DEFAULT_SCREEN_WIDTH,
                                         DEFAULT_SCREEN_HEIGHT));

    if (!sdl_texture_)
        throw std::runtime_error("Failed to create texture.");

    SDL_SetTextureScaleMode(sdl_texture_.get(), SDL_SCALEMODE_NEAREST);
}

Nes::~Nes()
{
    SDL_Quit();
}

void Nes::load_rom(const std::string& path)
{
    bus_->set_cartridge(std::make_shared<NesCartridge>(path));
}

void Nes::load_rom(const std::vector<uint8_t>& rom)
{
    bus_->set_cartridge(std::make_shared<NesCartridge>(rom));
}

void Nes::reset()
{
    bus_->reset();
    cpu_->reset();
}

void Nes::run()
{
    while (true)
    {
        if (!process_inputs())
            return;

        bus_->write8(0x00FE, std::rand() % 16u + 1u);

        render_screen();

        if (!cpu_->step())
            return;
    }
}

bool Nes::process_inputs()
{
    while (SDL_PollEvent(&sdl_event_))
    {
        if (sdl_event_.type == SDL_EVENT_QUIT)
        {
            return false;
        }
        else if (sdl_event_.type == SDL_EVENT_KEY_DOWN)
        {
            auto it = key_map_.find(sdl_event_.key.scancode);
            if (it != key_map_.end())
                bus_->write8(0xFF, it->second);
        }
    }

    return true;
}

void Nes::render_screen()
{
    bool update = false;

    for (uint16_t i = 0x0200; i < 0x0600; i++)
    {
        uint8_t color_idx = bus_->read8(i);
        uint32_t rgb      = COLORS.at(color_idx % 16u);

        if (screen_.at(i - 0x0200) != rgb)
        {
            screen_.at(i - 0x0200) = rgb;
            update                 = true;
        }
    }

    if (update)
    {
        SDL_UpdateTexture(sdl_texture_.get(),
                          nullptr,
                          screen_.data(),
                          DEFAULT_SCREEN_WIDTH * sizeof(uint32_t));

        SDL_RenderClear(sdl_renderer_.get());
        SDL_RenderTexture(sdl_renderer_.get(), sdl_texture_.get(), nullptr, nullptr);
        SDL_RenderPresent(sdl_renderer_.get());
    }
}
