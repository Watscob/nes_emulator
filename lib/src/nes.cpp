#include "nes.hpp"
#include <stdexcept>

Nes::Nes(size_t screen_scale)
    : bus_(std::make_shared<NesBus>())
    , cpu_(std::make_shared<NesCpu>())
    , joypad_1_(std::make_shared<NesJoypad>())
    , ppu_(std::make_shared<NesPpu>())
{
    connect_components_();

    size_t frame_width  = ppu_->get_frame_width();
    size_t frame_height = ppu_->get_frame_height();

    if (!SDL_Init(SDL_INIT_VIDEO))
        throw std::runtime_error("Fail to init SDL.");

    sdl_window_.reset(SDL_CreateWindow("NES Emulator",
                                       frame_width * screen_scale,
                                       frame_height * screen_scale,
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
                                         SDL_PIXELFORMAT_RGBA8888,
                                         SDL_TEXTUREACCESS_STREAMING,
                                         frame_width,
                                         frame_height));

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
    std::shared_ptr<NesCartridge> cartridge = std::make_shared<NesCartridge>(path);
    bus_->connect_cartridge(cartridge);
    ppu_->connect_cartridge(cartridge);
}

void Nes::load_rom(const std::vector<uint8_t>& rom)
{
    std::shared_ptr<NesCartridge> cartridge = std::make_shared<NesCartridge>(rom);
    bus_->connect_cartridge(cartridge);
    ppu_->connect_cartridge(cartridge);
}

void Nes::reset()
{
    bus_->reset();
    cpu_->reset();
    joypad_1_->reset();
    ppu_->reset();
}

void Nes::run()
{
    while (true)
    {
        if (!process_inputs_())
            return;

        if (!cpu_->step())
            return;

        render_screen_();
    }
}

void Nes::connect_components_()
{
    bus_->connect_ppu(ppu_);
    bus_->connect_joypad_1(joypad_1_);
    cpu_->connect_bus(bus_);
}

bool Nes::process_inputs_()
{
    while (SDL_PollEvent(&sdl_event_))
    {
        if (sdl_event_.type == SDL_EVENT_QUIT)
        {
            return false;
        }
        else
        {
            auto it = key_map_.find(sdl_event_.key.scancode);
            if (it != key_map_.end())
                joypad_1_->set_button_pressed(it->second, sdl_event_.type == SDL_EVENT_KEY_DOWN);
        }
    }

    return true;
}

void Nes::render_screen_()
{
    if (ppu_->poll_frame_ready())
    {
        const auto& frame = ppu_->get_frame();

        SDL_UpdateTexture(sdl_texture_.get(),
                          nullptr,
                          frame.data(),
                          ppu_->get_frame_width() * sizeof(uint32_t));

        SDL_RenderClear(sdl_renderer_.get());
        SDL_RenderTexture(sdl_renderer_.get(), sdl_texture_.get(), nullptr, nullptr);
        SDL_RenderPresent(sdl_renderer_.get());
    }
}
