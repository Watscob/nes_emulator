#ifndef NES_HPP
#define NES_HPP

#include <SDL3/SDL.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "nes_bus.hpp"
#include "nes_cpu.hpp"
#include "nes_ppu.hpp"

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

class Nes
{
  private:
    static constexpr size_t DEFAULT_SCREEN_SCALE = 5u;

    std::unordered_map<SDL_Scancode, uint8_t> key_map_ = {
        {SDL_SCANCODE_W, 0x77},
        {SDL_SCANCODE_S, 0x73},
        {SDL_SCANCODE_A, 0x61},
        {SDL_SCANCODE_D, 0x64},
    };

  public:
    explicit Nes(size_t screen_scale = DEFAULT_SCREEN_SCALE);
    ~Nes();

    void load_rom(const std::string& path);
    void load_rom(const std::vector<uint8_t>& rom);
    void reset();
    void run();

  private:
    std::shared_ptr<NesBus> bus_;
    std::shared_ptr<NesCpu> cpu_;
    std::shared_ptr<NesPpu> ppu_;
    SDL_Event sdl_event_;
    SDL_RendererPtr sdl_renderer_;
    SDL_WindowPtr sdl_window_;
    SDL_TexturePtr sdl_texture_;

    void connect_components_();
    bool process_inputs_();
    void render_screen_();
};

#endif /* NES_HPP */
