#include "Log.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <exception>

class SDLContextWrapper
{
public:
    SDLContextWrapper();

    ~SDLContextWrapper();

    const char* GetError() const;
};