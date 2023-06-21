#include "SDLContextWrapper.hpp"

#include "../Log.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <exception>

SDLContextWrapper::SDLContextWrapper(uint32_t sdlInitFlags)
{
    int result = SDL_Init(sdlInitFlags);

    if(result != 0)
    {
        Log.Error("Failed to initialize SDL", GetError());
        throw std::runtime_error("Failed to initialize SDL");
    }
}

SDLContextWrapper::~SDLContextWrapper()
{
    SDL_Vulkan_UnloadLibrary();
    SDL_Quit();
}

const char* SDLContextWrapper::GetError() const
{
    return SDL_GetError();
}