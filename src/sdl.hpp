#ifndef SDL_HPP
#define SDL_HPP

#include <SDL3/SDL.h>
#include <memory>
#include <nes.hpp>

struct SDL_RendererDeleter
{
    void operator()(SDL_Renderer* renderer) const
    {
        if (renderer)
            SDL_DestroyRenderer(renderer);
    }
};

struct SDL_TextureDeleter
{
    void operator()(SDL_Texture* texture) const
    {
        if (texture)
            SDL_DestroyTexture(texture);
    }
};

struct SDL_WindowDeleter
{
    void operator()(SDL_Window* window) const
    {
        if (window)
            SDL_DestroyWindow(window);
    }
};

using SDL_RendererPtr = std::unique_ptr<SDL_Renderer, SDL_RendererDeleter>;
using SDL_TexturePtr  = std::unique_ptr<SDL_Texture, SDL_TextureDeleter>;
using SDL_WindowPtr   = std::unique_ptr<SDL_Window, SDL_WindowDeleter>;

class Sdl
{
  public:
    enum class Key
    {
        W,
        S,
        A,
        D,
        QUIT
    };

    Sdl(uint32_t full_width, uint32_t full_height);
    Sdl(uint32_t full_width, uint32_t full_height, uint32_t scale_width, uint32_t scale_height);
    ~Sdl();

    bool init();
    std::vector<Key> get_inputs();
    uint32_t get_color(uint8_t color_idx);
    bool render(std::vector<uint32_t> frame);

    uint32_t get_width() { return width_; }
    uint32_t get_height() { return height_; }

  private:
    static constexpr uint32_t DEFAULT_SCALE = 10;

    uint32_t width_;
    uint32_t height_;
    uint32_t scale_width_;
    uint32_t scale_height_;
    SDL_Event event_;
    SDL_RendererPtr renderer_;
    SDL_WindowPtr window_;
    SDL_TexturePtr texture_;
};

#endif /* SDL_HPP */
