#pragma once

#include <SDL3/SDL.h>

#include <vector>

class SDLContextWrapper
{
public:
    explicit SDLContextWrapper(uint32_t sdl_initFlags);

    void EnableVulkan();
    std::vector<const char*> GetVulkanInstanceExtensions();
    
    ~SDLContextWrapper();

    const char* GetError() const;
};