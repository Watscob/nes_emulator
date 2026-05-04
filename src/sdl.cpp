#include "sdl.hpp"
#include <log.hpp>

Sdl::Sdl(uint32_t full_width, uint32_t full_height)
    : Sdl(full_width, full_height, DEFAULT_SCALE, DEFAULT_SCALE)
{
}

Sdl::Sdl(uint32_t full_width, uint32_t full_height, uint32_t scale_width, uint32_t scale_height)
    : width_(full_width / scale_width)
    , height_(full_height / scale_height)
    , scale_width_(scale_width)
    , scale_height_(scale_height)
{
}

Sdl::~Sdl()
{
    SDL_Quit();
}

bool Sdl::init()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        log_error("Fail to init SDL ({}).", SDL_GetError());
        return false;
    }

    window_.reset(SDL_CreateWindow("NES Emulator",
                                   width_ * scale_width_,
                                   height_ * scale_height_,
                                   SDL_WINDOW_RESIZABLE));

    if (!window_)
    {
        log_error("Failed to create a window ({}).", SDL_GetError());
        SDL_Quit();
        return false;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, window_.get());
    SDL_SetNumberProperty(props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, 1);
    renderer_.reset(SDL_CreateRendererWithProperties(props));

    if (!renderer_)
    {
        log_error("Failed to create a render ({}).", SDL_GetError());
        SDL_Quit();
        return false;
    }

    if (!SDL_SetRenderScale(renderer_.get(), scale_width_, scale_height_))
    {
        log_error("Failed to scale renderer ({}).", SDL_GetError());
        SDL_Quit();
        return false;
    }

    texture_.reset(SDL_CreateTexture(renderer_.get(),
                                     SDL_PIXELFORMAT_RGBA32,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     width_,
                                     height_));

    if (!texture_)
    {
        log_error("Failed to create texture ({}).", SDL_GetError());
        SDL_Quit();
        return false;
    }

    SDL_SetTextureScaleMode(texture_.get(), SDL_SCALEMODE_NEAREST);

    return true;
}

std::vector<Sdl::Key> Sdl::get_inputs()
{
    std::vector<Sdl::Key> keys;
    while (SDL_PollEvent(&event_))
    {
        if (event_.type == SDL_EVENT_QUIT)
        {
            keys.push_back(Sdl::Key::QUIT);
            return keys;
        }
        else if (event_.type == SDL_EVENT_KEY_DOWN)
        {
            switch (event_.key.key)
            {
            case SDLK_W:
                keys.push_back(Sdl::Key::W);
                break;
            case SDLK_S:
                keys.push_back(Sdl::Key::S);
                break;
            case SDLK_A:
                keys.push_back(Sdl::Key::A);
                break;
            case SDLK_D:
                keys.push_back(Sdl::Key::D);
                break;
            default:
                break;
            }
        }
    }

    return keys;
}

uint32_t Sdl::get_color(uint8_t color_idx)
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

bool Sdl::render(std::vector<uint32_t> frame)
{
    if (frame.size() != width_ * height_)
    {
        log_error("Frame size does not match screen size.");
        return false;
    }

    SDL_UpdateTexture(texture_.get(), nullptr, frame.data(), width_ * sizeof(uint32_t));

    SDL_RenderClear(renderer_.get());
    SDL_RenderTexture(renderer_.get(), texture_.get(), nullptr, nullptr);
    SDL_RenderPresent(renderer_.get());

    return true;
}
