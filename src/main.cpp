#include "Log.hpp"

#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <memory>

void Cleanup()
{
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << "Vulkan extension count: " << extensionCount << "\n";

    if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        LOG("SDL Initialized")
    }
    else
    {
        ERROR(SDL_GetError())
        return (EXIT_FAILURE);
    }

    int windowWidth = 720;
    int windowHeight = 300;
    SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;
    SDL_Window* window = SDL_CreateWindow("Vulkan-triangle", windowWidth, windowHeight, windowFlags);

    if(window != nullptr)
    {
        LOG("Window created")
    }
    else
    {
        ERROR(SDL_GetError())
        return (EXIT_FAILURE);
    }

    bool open = true;

    while(open)
    {
        SDL_Event e;

        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    open = false;
                break;
            }

            // Rendering thingies?
        }
    }
    
    return (EXIT_SUCCESS);
}