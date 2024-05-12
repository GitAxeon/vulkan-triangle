#include "SDLContextWrapper.hpp"

#include "../debug/Log.hpp"

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

void SDLContextWrapper::EnableVulkan()
{
    if(SDL_Vulkan_LoadLibrary(nullptr) != 0)
    {
        Log.Error("Failed to load Vulkan");
        throw std::runtime_error("Failed to load Vulkan");
    }

    Log.Info("Vulkan Library enabled");
}

std::vector<const char*> SDLContextWrapper::GetVulkanInstanceExtensions()
{
    uint32_t extensionCount = 0;
    const char* const* instanceExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);

    if(instanceExtensions == nullptr)
    {
        Log.Error("SDL_Vulkan_GetInstanceExtensions failed");
        Log.Error("SDL Error:" , SDL_GetError());

        throw std::runtime_error("Failed to retrieve Vulkan instance extensions");
    }

    std::vector<const char*> result;
    result.reserve(extensionCount);

    for(int i = 0; i < extensionCount; i++)
    {
        size_t length = strlen(instanceExtensions[i]);
        char* extensionCopy = new char[length + 1];
        strcpy(extensionCopy, instanceExtensions[i]);

        result.push_back(extensionCopy);
    }
    
    return result;
}

const char* SDLContextWrapper::GetError() const
{
    return SDL_GetError();
}