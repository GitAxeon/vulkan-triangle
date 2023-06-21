#pragma once

#include "SDL3/SDL_init.h"

class SDLContextWrapper
{
public:
    explicit SDLContextWrapper(uint32_t sdl_initFlags = SDL_INIT_EVERYTHING);
    ~SDLContextWrapper();

    const char* GetError() const;
};