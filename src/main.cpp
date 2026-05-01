#include <array>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <memory>
#include <vector>
#include "cpu.hpp"
#include "log.hpp"
#include "sdl_utils.hpp"

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

constexpr int SCREEN_WIDTH = 32;
constexpr int SCREEN_HEIGHT = 32;

constexpr int SCREEN_WIDTH_SCALE = 10;
constexpr int SCREEN_HEIGHT_SCALE = 10;

static std::array<uint32_t, SCREEN_WIDTH * SCREEN_HEIGHT> frame;

static std::unique_ptr<SDL_Event> event;
static SDL_RendererPtr renderer;
static SDL_WindowPtr window;
static SDL_TexturePtr texture;

static bool quit = false;

#if __EMSCRIPTEN__
EM_JS(void, sdl_init_canvas, (int width, int height), {
    const canvas = document.getElementById('canvas');
    canvas.width = width;
    canvas.height = height;
    canvas.style.width = width + 'px';
    canvas.style.height = height + 'px';
    canvas.style.display = 'block';

    const loading = document.getElementById('loading');
    loading.style.display = 'none';
});
#endif

static int sdl_init()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        log_error("Fail to init SDL ({}).", SDL_GetError());
        return 1;
    }

#if __EMSCRIPTEN__
    sdl_init_canvas(SCREEN_WIDTH * SCREEN_WIDTH_SCALE, SCREEN_HEIGHT * SCREEN_WIDTH_SCALE);
#endif

    window.reset(SDL_CreateWindow("NES Emulator",
                                  SCREEN_WIDTH * SCREEN_WIDTH_SCALE,
                                  SCREEN_HEIGHT * SCREEN_WIDTH_SCALE,
                                  SDL_WINDOW_RESIZABLE));

    if (!window)
    {
        log_error("Failed to create a window ({}).", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, window.get());
    SDL_SetNumberProperty(props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, 1);
    renderer.reset(SDL_CreateRendererWithProperties(props), SDL_RendererDeleter{});

    if (!renderer)
    {
        log_error("Failed to create a render ({}).", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    if (!SDL_SetRenderScale(renderer.get(), SCREEN_WIDTH_SCALE, SCREEN_HEIGHT_SCALE))
    {
        log_error("Failed to scale renderer ({}).", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    texture.reset(SDL_CreateTexture(renderer.get(),
                                    SDL_PIXELFORMAT_RGBA32,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    SCREEN_WIDTH,
                                    SCREEN_HEIGHT));

    if (!texture)
    {
        log_error("Failed to create texture ({}).", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_SetTextureScaleMode(texture.get(), SDL_SCALEMODE_NEAREST);

    event = std::make_unique<SDL_Event>();

    return 0;
}

static void sdl_handle_input(Cpu& cpu)
{
    while (SDL_PollEvent(event.get()))
    {
        if (event->type == SDL_EVENT_QUIT)
        {
            quit = true;
        }
        else if (event->type == SDL_EVENT_KEY_DOWN)
        {
            switch (event->key.key)
            {
            case SDLK_W:
                cpu.memory_->write8(0xFF, 0x77);
                break;
            case SDLK_S:
                cpu.memory_->write8(0xFF, 0x73);
                break;
            case SDLK_A:
                cpu.memory_->write8(0xFF, 0x61);
                break;
            case SDLK_D:
                cpu.memory_->write8(0xFF, 0x64);
                break;
            default:
                break;
            }
        }
    }
}

static uint32_t color(uint8_t color_idx)
{
    switch (color_idx)
    {
    /* Black */
    case 0u:
        return SDL_MapRGB(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32), nullptr, 0u, 0u, 0u);
    /* White */
    case 1u:
        return SDL_MapRGB(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32),
                          nullptr,
                          255u,
                          255u,
                          255u);
    /* Grey */
    case 2u:
    case 9u:
        return SDL_MapRGB(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32),
                          nullptr,
                          128u,
                          128u,
                          128u);
    /* Red */
    case 3u:
    case 10u:
        return SDL_MapRGB(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32), nullptr, 255u, 0u, 0u);
    /* Green */
    case 4u:
    case 11u:
        return SDL_MapRGB(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32), nullptr, 0u, 255u, 0u);
    /* Blue */
    case 5u:
    case 12u:
        return SDL_MapRGB(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32), nullptr, 0u, 0u, 255u);
    /* Magenta */
    case 6u:
    case 13u:
        return SDL_MapRGB(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32),
                          nullptr,
                          255u,
                          0u,
                          255u);
    /* Yellow */
    case 7u:
    case 14u:
        return SDL_MapRGB(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32),
                          nullptr,
                          255u,
                          255u,
                          0u);
    /* Cyan */
    default:
        return SDL_MapRGB(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32),
                          nullptr,
                          0u,
                          255u,
                          255u);
    }
}

static bool sdl_read_screen_state(Cpu& cpu,
                                  std::array<uint32_t, SCREEN_WIDTH * SCREEN_HEIGHT>& frame)
{
    bool update = false;

    for (uint16_t i = 0x0200; i < 0x0600; i++)
    {
        uint8_t color_idx = cpu.memory_->read8(i);
        uint32_t rgb = color(color_idx);

        if (frame.at(i - 0x0200) != rgb)
        {
            frame.at(i - 0x0200) = rgb;
            update = true;
        }
    }

    return update;
}

static void sdl_callback(Cpu& cpu)
{
    sdl_handle_input(cpu);
    cpu.memory_->write8(0x00FE, std::rand() % 16 + 1);

    bool updated = sdl_read_screen_state(cpu, frame);
    if (updated)
    {
        SDL_UpdateTexture(texture.get(), nullptr, frame.data(), SCREEN_WIDTH * sizeof(uint32_t));

        SDL_RenderClear(renderer.get());
        SDL_RenderTexture(renderer.get(), texture.get(), nullptr, nullptr);
        SDL_RenderPresent(renderer.get());
    }
}

#if __EMSCRIPTEN__
static void emscripten_loop_wrapper(void* arg)
{
    Cpu* nes = static_cast<Cpu*>(arg);
    if (quit)
    {
        emscripten_cancel_main_loop();
        return;
    }

    for (int i = 0; i < 100; i++)
    {
        if (!nes->execute_with_callback(*sdl_callback))
        {
            quit = true;
            break;
        }
    }
}
#endif

int main()
{
    set_log_level(Logger::LogLevel::DEBUG);
    log_info("NES emulator running ...");

    std::vector<uint8_t> rom = {
        0x20, 0x06, 0x06, 0x20, 0x38, 0x06, 0x20, 0x0d, 0x06, 0x20, 0x2a, 0x06, 0x60, 0xa9, 0x02,
        0x85, 0x02, 0xa9, 0x04, 0x85, 0x03, 0xa9, 0x11, 0x85, 0x10, 0xa9, 0x10, 0x85, 0x12, 0xa9,
        0x0f, 0x85, 0x14, 0xa9, 0x04, 0x85, 0x11, 0x85, 0x13, 0x85, 0x15, 0x60, 0xa5, 0xfe, 0x85,
        0x00, 0xa5, 0xfe, 0x29, 0x03, 0x18, 0x69, 0x02, 0x85, 0x01, 0x60, 0x20, 0x4d, 0x06, 0x20,
        0x8d, 0x06, 0x20, 0xc3, 0x06, 0x20, 0x19, 0x07, 0x20, 0x20, 0x07, 0x20, 0x2d, 0x07, 0x4c,
        0x38, 0x06, 0xa5, 0xff, 0xc9, 0x77, 0xf0, 0x0d, 0xc9, 0x64, 0xf0, 0x14, 0xc9, 0x73, 0xf0,
        0x1b, 0xc9, 0x61, 0xf0, 0x22, 0x60, 0xa9, 0x04, 0x24, 0x02, 0xd0, 0x26, 0xa9, 0x01, 0x85,
        0x02, 0x60, 0xa9, 0x08, 0x24, 0x02, 0xd0, 0x1b, 0xa9, 0x02, 0x85, 0x02, 0x60, 0xa9, 0x01,
        0x24, 0x02, 0xd0, 0x10, 0xa9, 0x04, 0x85, 0x02, 0x60, 0xa9, 0x02, 0x24, 0x02, 0xd0, 0x05,
        0xa9, 0x08, 0x85, 0x02, 0x60, 0x60, 0x20, 0x94, 0x06, 0x20, 0xa8, 0x06, 0x60, 0xa5, 0x00,
        0xc5, 0x10, 0xd0, 0x0d, 0xa5, 0x01, 0xc5, 0x11, 0xd0, 0x07, 0xe6, 0x03, 0xe6, 0x03, 0x20,
        0x2a, 0x06, 0x60, 0xa2, 0x02, 0xb5, 0x10, 0xc5, 0x10, 0xd0, 0x06, 0xb5, 0x11, 0xc5, 0x11,
        0xf0, 0x09, 0xe8, 0xe8, 0xe4, 0x03, 0xf0, 0x06, 0x4c, 0xaa, 0x06, 0x4c, 0x35, 0x07, 0x60,
        0xa6, 0x03, 0xca, 0x8a, 0xb5, 0x10, 0x95, 0x12, 0xca, 0x10, 0xf9, 0xa5, 0x02, 0x4a, 0xb0,
        0x09, 0x4a, 0xb0, 0x19, 0x4a, 0xb0, 0x1f, 0x4a, 0xb0, 0x2f, 0xa5, 0x10, 0x38, 0xe9, 0x20,
        0x85, 0x10, 0x90, 0x01, 0x60, 0xc6, 0x11, 0xa9, 0x01, 0xc5, 0x11, 0xf0, 0x28, 0x60, 0xe6,
        0x10, 0xa9, 0x1f, 0x24, 0x10, 0xf0, 0x1f, 0x60, 0xa5, 0x10, 0x18, 0x69, 0x20, 0x85, 0x10,
        0xb0, 0x01, 0x60, 0xe6, 0x11, 0xa9, 0x06, 0xc5, 0x11, 0xf0, 0x0c, 0x60, 0xc6, 0x10, 0xa5,
        0x10, 0x29, 0x1f, 0xc9, 0x1f, 0xf0, 0x01, 0x60, 0x4c, 0x35, 0x07, 0xa0, 0x00, 0xa5, 0xfe,
        0x91, 0x00, 0x60, 0xa6, 0x03, 0xa9, 0x00, 0x81, 0x10, 0xa2, 0x00, 0xa9, 0x01, 0x81, 0x10,
        0x60, 0xa2, 0x00, 0xea, 0xea, 0xca, 0xd0, 0xfb, 0x60};

    std::srand(std::time({}));

    Cpu nes;
    nes.load(rom, 0x0600);
    nes.reset();

    if (sdl_init() != 0)
        return 1;

#if __EMSCRIPTEN__
    emscripten_set_main_loop_arg(&emscripten_loop_wrapper, &nes, 0, true);
#else
    while (!quit && nes.execute_with_callback(sdl_callback)) {}
#endif

    log_info("NES emulator exiting ...");

    SDL_Quit();
    return 0;
}
