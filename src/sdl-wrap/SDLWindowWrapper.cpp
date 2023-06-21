#include "SDLWindowWrapper.hpp"

SDLWindowWrapper::SDLWindowWrapper(const WindowInfo& info)
{
    uint32_t flags = SDL_WINDOW_VULKAN;
    m_SDLWindow = SDL_CreateWindow(info.Title.c_str(), info.Width, info.Height, flags);

    if(m_SDLWindow == nullptr)
    {
        Log.Error("Created SDLWindowWrapper unsuccesfully");
        throw std::runtime_error("Failed to create Window");
    }

    Log.Info("Created SDLWindowWrapper successfully");
}

SDLWindowWrapper::~SDLWindowWrapper()
{
    Log.Info("Destructed SDLWindowWrapper");
    SDL_DestroyWindow(m_SDLWindow);
}

SDL_Window* SDLWindowWrapper::GetNativeWindow() const
{
    return m_SDLWindow;
}