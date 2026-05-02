#include <array>
#include <bus.hpp>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <log.hpp>
#include <memory>
#include <nes.hpp>
#include <vector>
#include "sdl_utils.hpp"

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

constexpr int SCREEN_WIDTH  = 32;
constexpr int SCREEN_HEIGHT = 32;

constexpr int SCREEN_WIDTH_SCALE  = 10;
constexpr int SCREEN_HEIGHT_SCALE = 10;

static std::array<uint32_t, SCREEN_WIDTH * SCREEN_HEIGHT> frame;

static std::unique_ptr<SDL_Event> event;
static SDL_RendererPtr renderer;
static SDL_WindowPtr window;
static SDL_TexturePtr texture;

static bool quit = false;

#if __EMSCRIPTEN__
EM_JS(void, sdl_init_canvas, (int width, int height), {
    const canvas         = document.getElementById('canvas');
    canvas.width         = width;
    canvas.height        = height;
    canvas.style.width   = width + 'px';
    canvas.style.height  = height + 'px';
    canvas.style.display = 'block';

    const loading         = document.getElementById('loading');
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

static void sdl_handle_input(Nes& nes)
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
                nes.get_bus()->write8(0xFF, 0x77);
                break;
            case SDLK_S:
                nes.get_bus()->write8(0xFF, 0x73);
                break;
            case SDLK_A:
                nes.get_bus()->write8(0xFF, 0x61);
                break;
            case SDLK_D:
                nes.get_bus()->write8(0xFF, 0x64);
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

static bool sdl_read_screen_state(Nes& nes,
                                  std::array<uint32_t, SCREEN_WIDTH * SCREEN_HEIGHT>& frame)
{
    bool update = false;

    for (uint16_t i = 0x0200; i < 0x0600; i++)
    {
        uint8_t color_idx = nes.get_bus()->read8(i);
        uint32_t rgb      = color(color_idx);

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
    sdl_handle_input(nes);
    nes.get_bus()->write8(0x00FE, std::rand() % 16 + 1);

    bool updated = sdl_read_screen_state(nes, frame);
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
    Nes* nes = static_cast<Nes*>(arg);
    if (quit)
    {
        emscripten_cancel_main_loop();
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
#endif

int main()
{
    set_log_level(Logger::LogLevel::DEBUG);
    log_info("NES emulator running ...");

    std::srand(std::time({}));

    Nes nes(sdl_callback, "roms/snake.nes");
    nes.reset();

    if (sdl_init() != 0)
        return 1;

#if __EMSCRIPTEN__
    emscripten_set_main_loop_arg(&emscripten_loop_wrapper, &nes, 0, true);
#else
    while (!quit && nes.step()) {}
#endif

    log_info("NES emulator exiting ...");

    SDL_Quit();
    return 0;
}
