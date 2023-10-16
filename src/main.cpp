#include "application/sdl-wrap/SDLContextWrapper.hpp"
#include "application/Window.hpp"

#include "application/Vulkan/VulkanInstance.hpp"
#include "application/Vulkan/DebugUtilsMessenger.hpp"
#include "application/Vulkan/VulkanDeviceSelector.hpp"
#include "application/Vulkan/VulkanDevice.hpp"
#include "application/Vulkan/VulkanSwapchain.hpp"

#include "application/RenderingContext.hpp"

#include <SDL3/SDL.h>
#include <glm/vec2.hpp>
#include <glm/mat2x2.hpp>

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, 
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

void RunApplication();

int main(int argc, char* argv[])
{
    try
    {
        RunApplication();
    }
    catch(const std::exception& exception)
    {
        Log.Error(exception.what());
        
        return (EXIT_FAILURE);
    }
        
    return (EXIT_SUCCESS);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, 
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    const char* msg = "Vulkan validation layer: ";

    switch(messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            Log.Error(msg, pCallbackData->pMessage, "\n");
        break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            Log.Warn(msg, pCallbackData->pMessage, "\n");
        break;
        default: //VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            Log.Info(msg, pCallbackData->pMessage, "\n");
        break;
    }

    return VK_FALSE;
}

void RunApplication()
{
    WindowInfo info("Vulkan-triangle", 720, 300);

    SDLContextWrapper SDLContext;
    Log.Info("SDLContext Initialized");
    SDLContext.EnableVulkan();
    
    VulkanInstanceCreateInfo createInfo;
    createInfo.ApplicationName = info.Title;
    createInfo.EnableValidationLayers = true;
    createInfo.Extensions = SDLContext.GetVulkanInstanceExtensions();
    createInfo.Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    createInfo.ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

    std::shared_ptr<VulkanInstance> vulkanInstance = std::make_shared<VulkanInstance>(createInfo);
    
    DebugUtilsMessenger debugMessenger(vulkanInstance, DebugCallback);

    Window window(info);
    RenderingContext renderingContext(window, vulkanInstance);

    VulkanQueueRequest req1;
    req1.Flags = VK_QUEUE_GRAPHICS_BIT;
    req1.Surface = renderingContext.GetSurface();
    req1.Count = 1;

    VulkanDeviceRequirements requirements;
    requirements.Queues.push_back(req1);
    requirements.Extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    Log.Info("Creating device selector");
    std::shared_ptr<VulkanDeviceSelector> selector = std::make_shared<VulkanDeviceSelector>(vulkanInstance, requirements);
    
    Log.Info("Creating logical device");
    std::shared_ptr<VulkanDevice> device = selector->GetDevice();

    Log.Info("Requesting test queue");
    VkQueue graphicsQueue = device->GetQueue(req1, 0);

    SwapchainSupportDetails swapchainDetails = device->GetSwapchainSupportDetails(renderingContext.GetSurface());

    VulkanSwapchainPreferences swapchainPreferences;
    swapchainPreferences.SurfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
    swapchainPreferences.SurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchainPreferences.PresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

    // VkFormat preferredFormat = VK_FORMAT_B8G8R8A8_SRGB;
    // VkColorSpaceKHR preferredColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    // auto surfaceFormatIterator = std::find_if(swapchainDetails.Formats.begin(), swapchainDetails.Formats.end(), [&preferredFormat, &preferredColorSpace](const VkSurfaceFormatKHR& format)
    // {
    //     return format.format == preferredFormat && format.colorSpace == preferredColorSpace;
    // });

    // if(surfaceFormatIterator == swapchainDetails.Formats.end())
    //     Log.Warn("Device doesn't support preferred VkSurfaceFormat");
    // else
    //     Log.Info("A VkSurfaceFormat supporting preferred settings was found");

    // VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    // auto presentModeIterator = std::find_if(swapchainDetails.PresentModes.begin(), swapchainDetails.PresentModes.end(), [&preferredPresentMode](const VkPresentModeKHR& presentMode)
    // {
    //     return presentMode == preferredPresentMode;
    // });

    // if(presentModeIterator == swapchainDetails.PresentModes.end())
    //     Log.Warn("Device doesn't support preferred VkPresentMode");
    // else
    //     Log.Info("Device supports preferred present mode");

    // VkSurfaceFormatKHR surfaceFormat;
    // VkPresentModeKHR presentMode;

    VulkanSwapchain swapchain(device, renderingContext.GetSurface(), swapchainPreferences);

    Log.Info("Entering EventLoop");
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
        }
        // Rendering thingies?
    
    }

    Log.Info("Exiting EventLoop");
}