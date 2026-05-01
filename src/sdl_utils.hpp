#ifndef SDL_UTILS
#define SDL_UTILS

#include <SDL3/SDL.h>
#include <memory>

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

using SDL_RendererPtr = std::shared_ptr<SDL_Renderer>;
using SDL_TexturePtr  = std::unique_ptr<SDL_Texture, SDL_TextureDeleter>;
using SDL_WindowPtr   = std::unique_ptr<SDL_Window, SDL_WindowDeleter>;

#endif /* SDL_UTILS */
