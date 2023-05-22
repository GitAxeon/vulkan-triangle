#include "Log.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>
#include <glm/mat2x2.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

static LoggerBase<std::ostream> Log(std::cout);

struct WindowInfo
{
    std::string Title;
    unsigned int Width;
    unsigned int Height;

    WindowInfo()
        : Title("default"), Width(720), Height(300) {}

    WindowInfo(const std::string& title, unsigned int width, unsigned int height)
        : Title(title), Width(width), Height(height) {}
};

class SDLContextWrapper
{
public:
    SDLContextWrapper() = default;
    ~SDLContextWrapper()
    {
        SDL_Vulkan_UnloadLibrary();
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
        uint32_t flags = SDL_WINDOW_VULKAN;
        m_SDLWindow = SDL_CreateWindow(info.Title.c_str(), info.Width, info.Height, flags);

        if(m_SDLWindow != nullptr)
            Log.Info("Created SDLWindowWrapper successfully");
        else
            Log.Error("Created SDLWindowWrapper unsuccesfully");
    }
    
    ~SDLWindowWrapper()
    {
        Log.Info("Destructed SDLWindowWrapper");
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

        Log.Info("Created Window");
    }

    ~Window()
    {
        Log.Info("Destructed window");
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

class VulkanContext
{
public:
    VulkanContext()
        : m_Instance(nullptr) {}

    uint32_t Init(const std::string& applicationName)
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = applicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        uint32_t SDLExtensionCount = 0;
        SDL_bool extensionResult = SDL_Vulkan_GetInstanceExtensions(&SDLExtensionCount, nullptr);

        if(extensionResult == SDL_FALSE)
        {
            Log.Error("Failed to retrieve necessary Vulkan extensions");
            return VK_ERROR_INITIALIZATION_FAILED;
        }
        else
        {
            Log.Info("Retrieved number of necessary extensions: ", SDLExtensionCount);
        }

        std::vector<const char*> SDLExtensions;
        SDLExtensions.resize(SDLExtensionCount);

        extensionResult = SDL_Vulkan_GetInstanceExtensions(&SDLExtensionCount, SDLExtensions.data());    
        
        if(extensionResult == SDL_FALSE)
        {
            Log.Error("Failed to retrieve necessary Vulkan extensions");
            return VK_ERROR_INITIALIZATION_FAILED;
        }
        else
        {
            Log.Info("Retrieved names of the extensions");

            for(size_t i = 0; i < SDLExtensionCount; i++)
            {
                Log.Info(i, ": ", SDLExtensions[i]);
            }
        }
        
        VkInstanceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.enabledExtensionCount = SDLExtensionCount;
        createInfo.ppEnabledExtensionNames = SDLExtensions.data();

        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);

        if(result == VK_SUCCESS)
        {
            Log.Info("Created Vulkan instance");
        }
        else
        {
            Log.Error("Failed to create Vulkan instance ", "VkResult: ", result);
        }
        
        return result;
    }

private:
    VkInstance m_Instance;
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
    Window window(info);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    Log.Info("Vulkan extension count: ", extensionCount);

    VulkanContext context;
    
    if(context.Init(info.Title) == VK_SUCCESS)
    {
        Log.Info("Created VulkanContext succesfully");
    }
    else
    {
        Log.Error("Failed to create VulkanContext");
        return (EXIT_FAILURE);
    }

    while(window.IsOpen())
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    window.Close();
                break;
                case SDL_EVENT_KEY_DOWN:
                {
                    if(e.key.keysym.sym == SDLK_ESCAPE)
                        window.Close();
                } break;
            }

            // Rendering thingies?
        }
    }

    return (EXIT_SUCCESS);
}