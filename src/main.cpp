#include "application/sdl-wrap/SDLContextWrapper.hpp"
#include "application/Window.hpp"

#include "application/Vulkan/VulkanInstance.hpp"
#include "application/Vulkan/DebugUtilsMessenger.hpp"
#include "application/Vulkan/VulkanDevice.hpp"

#include "application/RenderingContext.hpp"

#include <SDL3/SDL.h>
#include <glm/vec2.hpp>
#include <glm/mat2x2.hpp>

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, 
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        Log.Error("Vulkan validation layer: ", pCallbackData->pMessage);
    }

    return VK_FALSE;
}

int main(int argc, char* argv[])
{
    try
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
        // createInfo.Extensions.push_back("moi");

        createInfo.ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

        std::shared_ptr<VulkanInstance> vulkanInstance = std::make_shared<VulkanInstance>(createInfo);
        Log.Info("Created VulkanContext");
        
        DebugUtilsMessenger debugMessenger(vulkanInstance, DebugCallback);

        Window window(info);
        RenderingContext renderingContext(window, vulkanInstance);

        auto devices = VulkanPhysicalDevice::EnumeratePhysicalDevices(vulkanInstance);
        Log.Info("Number of available physical devices [", devices.size(), "]");

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

                // Rendering thingies?
            }
        }
        Log.Info("Exiting EventLoop");

        return (EXIT_SUCCESS);
    }
    catch(const std::exception& exception)
    {
        Log.Error(exception.what());
        
        return (EXIT_FAILURE);
    }
}

//     VkResult SelectPhysicalDevice()
//     {
//         m_PhysicalDevice = VK_NULL_HANDLE;

//         uint32_t deviceCount = 0;
//         vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

//         if(deviceCount == 0)
//         {
//             Log.Info("Failed to find GPUs with Vulkan support!");
//             return VK_ERROR_UNKNOWN;
//         }

//         std::vector<VkPhysicalDevice> devices(deviceCount);
//         vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

//         for(const auto& device : devices)
//         {
//             if(IsSuitableDevice(device))
//             {
//                 m_PhysicalDevice = device;
//                 break;
//             }
//         }

//         if(m_PhysicalDevice == VK_NULL_HANDLE)
//         {
//             Log.Error("Failed to find a suitable GPU!");
//             return VK_ERROR_UNKNOWN;
//         }

//         return VK_SUCCESS;
//     }

//     bool IsSuitableDevice(VkPhysicalDevice device)
//     {
//         QueueFamilyIndices indices = FindQueueFamilies(device);

//         return indices.IsComplete();
//     }

//     QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
//     {
//         QueueFamilyIndices indices;
        
//         uint32_t queueFamilyCount = 0;
//         vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

//         std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//         vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

//         int i = 0;
//         for(const auto& queueFamily : queueFamilies)
//         {
//             if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
//             {
//                 indices.GraphicsFamily = i;
//             }

//             // VkBool32 presentSupport = false;
//             // vkGetPhysicalDeviceSurfaceSupportKHR(device, i, )

//             if(indices.IsComplete())
//                 break;

//             ++i;
//         }

//         return indices;
//     }

//     uint32_t CreateLogicalDevice()
//     {
//         QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

//         VkDeviceQueueCreateInfo queueCreateInfo{};
//         queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//         queueCreateInfo.queueFamilyIndex = indices.GraphicsFamily.value();
//         queueCreateInfo.queueCount = 1;

//         float queuePriority = 1.0f;
//         queueCreateInfo.pQueuePriorities = &queuePriority;

//         VkPhysicalDeviceFeatures deviceFeatures{};

//         VkDeviceCreateInfo createInfo{};
//         createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//         createInfo.pQueueCreateInfos = &queueCreateInfo;
//         createInfo.queueCreateInfoCount = 1;
//         createInfo.pEnabledFeatures = &deviceFeatures;

//         createInfo.enabledExtensionCount = 0;
//         if(m_EnableValidationLayers)
//         {
//             createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
//             createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
//         }
//         else
//         {
//             createInfo.enabledLayerCount = 0;
//         }

//         VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);

//         if(result != VK_SUCCESS)
//         {
//             return VK_ERROR_INITIALIZATION_FAILED;
//         }

//         vkGetDeviceQueue(m_Device, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);

//         return VK_SUCCESS;
//     }

// private:
//     VkInstance m_Instance;
//     VkPhysicalDevice m_PhysicalDevice;
//     VkDevice m_Device;
//     VkQueue m_GraphicsQueue;
//     VkDebugUtilsMessengerEXT m_DebugMessenger;

//     bool m_EnableValidationLayers = false;

//     std::array<const char*, 1> m_ValidationLayers = 
//     {
//         "VK_LAYER_KHRONOS_validation"
//     };
// };