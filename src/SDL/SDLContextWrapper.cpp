#include "SDLContextWrapper.hpp"

SDLContextWrapper::SDLContextWrapper()
{
    uint32_t sdlInitFlags = SDL_INIT_EVERYTHING;

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