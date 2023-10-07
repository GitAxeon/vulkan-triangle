#include "application/sdl-wrap/SDLContextWrapper.hpp"
#include "application/Window.hpp"

#include "application/Vulkan/VulkanInstance.hpp"
#include "application/Vulkan/DebugUtilsMessenger.hpp"
#include "application/Vulkan/VulkanDeviceSelector.hpp"
#include "application/Vulkan/VulkanDevice.hpp"

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

    std::vector<VulkanQueueRequest> queueRequests;
    
    VulkanQueueRequest req1;
    req1.Flags = VK_QUEUE_GRAPHICS_BIT;
    req1.Surface = renderingContext.GetSurface();
    req1.Count = 1;
    queueRequests.emplace_back(req1);

    std::vector<std::string> requiredDeviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VulkanDeviceRequirements requirements;
    requirements.Queues.push_back(req1);
    requirements.Extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    Log.Info("Creating device selector");
    std::shared_ptr<VulkanDeviceSelector> selector = std::make_shared<VulkanDeviceSelector>(vulkanInstance, requiredDeviceExtensions, queueRequests);

    VulkanPhysicalDevice testDevice(vulkanInstance, VulkanPhysicalDevice::GetDevice(vulkanInstance, 0), 0);

    Log.Info("Creating logical device");
    std::shared_ptr<VulkanDevice> device = std::make_shared<VulkanDevice>(selector);

    VkQueue graphicsQueue = device->GetQueue(req1, 0);

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