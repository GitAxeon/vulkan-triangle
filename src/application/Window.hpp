#pragma once 
#include "sdl-wrap/SDLWindowWrapper.hpp"

#include <memory>

class Window
{
public:
    Window() = delete;    
    Window(const WindowInfo& info);
    ~Window();

    SDL_Window* GetNativeWindow() const;
    bool IsOpen() const;
    void Close();

    VkSurfaceKHR CreateSurface(std::shared_ptr<VulkanInstance> instance) const;

private:
    WindowInfo m_WindowInfo;
    std::unique_ptr<SDLWindowWrapper> m_Wrapper;

    bool m_Open = true;
};