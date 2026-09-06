#ifndef NES_HPP
#define NES_HPP

#include <SDL3/SDL.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "nes_bus.hpp"
#include "nes_cpu.hpp"
#include "nes_joypad.hpp"
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

    std::unordered_map<SDL_Scancode, NesJoypad::JoypadButton> key_map_ = {
        {SDL_SCANCODE_RIGHT, NesJoypad::JoypadButton::RIGHT},
        {SDL_SCANCODE_LEFT, NesJoypad::JoypadButton::LEFT},
        {SDL_SCANCODE_DOWN, NesJoypad::JoypadButton::DOWN},
        {SDL_SCANCODE_UP, NesJoypad::JoypadButton::UP},
        {SDL_SCANCODE_RETURN, NesJoypad::JoypadButton::START},
        {SDL_SCANCODE_SPACE, NesJoypad::JoypadButton::SELECT},
        {SDL_SCANCODE_S, NesJoypad::JoypadButton::B},
        {SDL_SCANCODE_A, NesJoypad::JoypadButton::A}};

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
    std::shared_ptr<NesJoypad> joypad_1_;
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
