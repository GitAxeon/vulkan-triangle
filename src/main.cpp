#include "Log.hpp"

#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>
#include <glm/mat2x2.hpp>

#include <iostream>
#include <memory>
#include <string>

static LoggerBase<std::ostream> Log(std::cout);

struct WindowInfo
{
    std::string Title;
    unsigned int Width;
    unsigned int Height;

    WindowInfo()
        : Title("default"), Width(720), Height(300) {}

    WindowInfo(std::string title, unsigned int width, unsigned int height)
        : Title(title), Width(width), Height(height) {}
};

class SDLContextWrapper
{
public:
    SDLContextWrapper() = default;
    ~SDLContextWrapper()
    {
        SDL_Quit();
    }

    bool Init(uint32_t sdlInitFlags = SDL_INIT_EVERYTHING) const
    {
        return SDL_Init(sdlInitFlags);
    }

    const char* GetError() const
    {
        return SDL_GetError();
    }
};

class SDLWindowWrapper
{
public:
    SDLWindowWrapper(WindowInfo& info)
    {
        SDL_WindowFlags flags = SDL_WINDOW_VULKAN;
        m_SDLWindow = SDL_CreateWindow(info.Title.c_str(), info.Width, info.Height, flags);

        if(m_SDLWindow != nullptr)
            Log.Info("SDLWindowWrapper created successfully");
        else
            Log.Error("SDLWindowWrapper created unsuccesfully");
    }
    
    ~SDLWindowWrapper()
    {
        Log.Info("Destructing SDLWindow");
        SDL_DestroyWindow(m_SDLWindow);
    }

    SDL_Window* GetNativeWindow() const
    {
        return m_SDLWindow;
    }

private:
    SDL_Window* m_SDLWindow;
};

class Window
{
public:
    Window(WindowInfo& info)
    {
        m_WindowInfo = info;
        m_Wrapper = std::make_unique<SDLWindowWrapper>(info);

        Log.Info("Window created");
    }

    ~Window()
    {
        Log.Info("Destructing window");
    }

    SDL_Window* GetNativeWindow() const
    {
        return m_Wrapper->GetNativeWindow();
    }

    bool IsOpen() const
    {
        return m_Open;
    }

    void Close()
    {
        m_Open = false;
    }

private:
    std::unique_ptr<SDLWindowWrapper> m_Wrapper;
    WindowInfo m_WindowInfo;

    bool m_Open = true;
};

int main(int argc, char* argv[])
{
    SDLContextWrapper SDLContext;

    if(SDLContext.Init() == 0)
    {
        Log.Info("SDLContext Initialized");
    }
    else
    {
        Log.Error(SDLContext.GetError());
        return (EXIT_FAILURE);
    }

    WindowInfo info("Vulkan-triangle", 720, 300);
    Window testWindow(info);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    Log.Info("Vulkan extension count:", 10);

    while(testWindow.IsOpen())
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    testWindow.Close();
                break;
            }

            // Rendering thingies?
        }
    }

    return (EXIT_SUCCESS);
}