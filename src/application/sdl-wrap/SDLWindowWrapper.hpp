#pragma once

#include "../debug/Log.hpp"
#include "../Vulkan/VulkanInstance.hpp"

#include <SDL3/SDL_vulkan.h>
#include <Vulkan/vulkan.hpp>

#include <string>

struct WindowInfo
{
    std::string Title;
    unsigned int Width;
    unsigned int Height;
    uint32_t Flags;

    WindowInfo()
        : Title("default"), Width(720), Height(300), Flags(0) {}

    WindowInfo(const std::string& title, unsigned int width, unsigned int height)
        : Title(title), Width(width), Height(height), Flags(0) {}

    WindowInfo(const std::string& title, unsigned int width, unsigned int height, uint32_t windowFlags)
        : Title(title), Width(width), Height(height), Flags(windowFlags) {}
};

class SDLWindowWrapper
{
public:
    SDLWindowWrapper(const WindowInfo& info);
    ~SDLWindowWrapper();

    VkSurfaceKHR CreateVulkanSurface(VkInstance instance);

    SDL_Window* GetNativeWindow() const;

private:
    SDL_Window* m_SDLWindow;
};