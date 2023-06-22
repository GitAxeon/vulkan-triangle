// #include "Log.hpp"

#include "sdl-wrap/SDLContextWrapper.hpp"
#include "sdl-wrap/SDLWindowWrapper.hpp"
#include "Window.hpp"

#include "Vulkan/VulkanInstance.hpp"
#include "Vulkan/VulkanRenderingContext.hpp"
#include "Vulkan/DebugUtilsMessenger.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
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


// static LoggerBase<std::ostream> Log(std::cout);

// class VulkanContext
// {
// public:
//     VulkanContext()
//         : m_Instance(nullptr), m_PhysicalDevice(nullptr), m_Device(nullptr), m_DebugMessenger(nullptr), m_EnableValidationLayers(true)
//     {
//         if(!Init("Default"))
//             throw std::runtime_error("Failed to initialize VulkanContext");
//     }

//     VulkanContext(const std::string& applicationName, bool enableValidationLayers)
//         : m_Instance(nullptr), m_PhysicalDevice(nullptr), m_Device(nullptr), m_DebugMessenger(nullptr), m_EnableValidationLayers(enableValidationLayers)
//     {
//         if(!Init(applicationName))
//             throw std::runtime_error("Failed to initialize VulkanContext");
//     }

//     ~VulkanContext()
//     {
//         if(m_EnableValidationLayers)
//         {
//             Log.Info("Destructed debug messenger");
//             DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
//         }

//         vkDestroyDevice(m_Device, nullptr);

//         Log.Info("Destructed VulkanContext");
//         vkDestroyInstance(m_Instance, nullptr);
//     }

//     VkInstance GetInstance() const
//     {
//         return m_Instance;
//     }
// private:
//     bool Init(const std::string& applicationName)
//     {
//         if(SDL_Vulkan_LoadLibrary(nullptr) != 0)
//         {
//             Log.Error("Failed to load Vulkan");
//             return false;
//         }

//         if(m_EnableValidationLayers && !CheckValidationLayerSupport())
//         {
//             Log.Error("Validation layers requested but not available");
//             return false;
//         }

//         VkApplicationInfo appInfo{};
//         appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//         appInfo.pApplicationName = applicationName.c_str();
//         appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
//         appInfo.pEngineName = "No engine";
//         appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
//         appInfo.apiVersion = VK_API_VERSION_1_1;

//         std::vector<const char*> SDLExtensions;
//         VkResult extensionResult = GetInstanceExtensions(SDLExtensions);

//         if(extensionResult != VK_SUCCESS)
//         {
//             return false;
//         }

//         VkInstanceCreateInfo createInfo {};
//         createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//         createInfo.pApplicationInfo = &appInfo;
//         createInfo.enabledExtensionCount = SDLExtensions.size();
//         createInfo.ppEnabledExtensionNames = SDLExtensions.data();

//         VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
//         if(m_EnableValidationLayers)
//         {
//             createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
//             createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

//             FillDebugMessengerCreateInfo(debugCreateInfo);
//             createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
//         }
//         else
//         {
//             createInfo.enabledLayerCount = 0;
//             createInfo.ppEnabledLayerNames = nullptr;
//         }

//         VkResult instanceResult = vkCreateInstance(&createInfo, nullptr, &m_Instance);

//         if(instanceResult != VK_SUCCESS)
//         {
//             Log.Error("Failed to create Vulkan instance ", "VkResult: ", instanceResult);
//             return false;
//         }

//         Log.Info("Created Vulkan instance");

//         if(SetupDebugMessenger())
//         {
//             Log.Info("Created Vulkan debug messenger");
//         }
//         else
//         {
//             Log.Warn("Debugging not enabled");
//         }

//         SelectPhysicalDevice();

//         CreateLogicalDevice();

//         return true;
//     }
    
//     VkResult GetInstanceExtensions(std::vector<const char*>& extensions)
//     {
//         uint32_t SDLExtensionCount = 0;
//         SDL_bool extensionResult = SDL_Vulkan_GetInstanceExtensions(&SDLExtensionCount, nullptr);

//         if(extensionResult == SDL_FALSE)
//         {
//             Log.Error("Failed to retrieve necessary Vulkan extensions");
//             Log.Error("SDL Error:" , SDL_GetError());
//             return VK_ERROR_EXTENSION_NOT_PRESENT;
//         }
//         else
//         {
//             Log.Info("Retrieved ", SDLExtensionCount, " extensions");
//         }

//         extensions.resize(SDLExtensionCount);

//         extensionResult = SDL_Vulkan_GetInstanceExtensions(&SDLExtensionCount, extensions.data()); 
        
//         if(extensionResult == SDL_FALSE)
//         {
//             Log.Error("Failed to retrieve necessary Vulkan extensions");
//             Log.Error("SDL Error:" , SDL_GetError());
//             return VK_ERROR_EXTENSION_NOT_PRESENT;
//         }

//         if(m_EnableValidationLayers)
//         {
//             Log.Info("Included debug messenger extension");
//             extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
//         }
        
//         for(size_t i = 0; i < extensions.size(); i++)
//         {
//             Log.Info(i + 1, ": ", extensions[i]);
//         }

//         return VK_SUCCESS;
//     }

//     bool CheckValidationLayerSupport() const
//     {
//         uint32_t layerCount = 0;
//         vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

//         std::vector<VkLayerProperties> availableLayers(layerCount);
//         vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

//         for(auto layerName : m_ValidationLayers)
//         {
//             bool layerFound = false;
            
//             for(const auto& layerProperties : availableLayers)
//             {
//                 if(strcmp(layerName, layerProperties.layerName) == 0)
//                 {
//                     Log.Info("Found validation layer \"", layerName, "\"");
//                     layerFound = true;
//                     break;
//                 }
//             }

//             if(!layerFound)
//             {
//                 Log.Error("Couldn't find validation layer \"", layerName, "\"");
//                 return false;
//             }
//         }

//         return true;
//     }

//     void FillDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& info)
//     {
//         info = {};
//         info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
//         info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
//         info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
//         info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
//         info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
//         info.pfnUserCallback = DebugCallback;
//         info.pUserData = nullptr;
//     }
    
//     bool SetupDebugMessenger()
//     {
//         if(!m_EnableValidationLayers) return false;

//         VkDebugUtilsMessengerCreateInfoEXT createInfo;
//         FillDebugMessengerCreateInfo(createInfo);

//         if(CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
//         {
//             return false;
//         }

//         return true;
//     }

//     VkResult CreateDebugUtilsMessengerEXT(
//         VkInstance instance,
//         const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
//         const VkAllocationCallbacks* pAllocator,
//         VkDebugUtilsMessengerEXT* pDebugMessenger)
//     {
//         auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

//         if(func != nullptr)
//         {
//             return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
//         }
//         else
//         {
//             return VK_ERROR_EXTENSION_NOT_PRESENT;
//         }
//     }

//     static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
//         VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//         VkDebugUtilsMessageTypeFlagsEXT messageType, 
//         const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//         void* pUserData)
//     {
//         if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
//         {
//             Log.Error("Vulkan validation layer: ", pCallbackData->pMessage);
//         }
//         return VK_FALSE;
//     }

//     void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
//     {
//         auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");

//         if(func != nullptr)
//         {
//             func(instance, debugMessenger, pAllocator);
//         }
//     }

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

// class RenderingContext
// {
// public:
//     RenderingContext(const Window& window, const std::shared_ptr<VulkanInstance>& context)
//     {
//         SDL_bool result = SDL_Vulkan_CreateSurface(window.GetNativeWindow(), context->GetInstance(), &m_Surface);

//         if(result == SDL_FALSE)
//             throw std::runtime_error("Failed to create rendering context");

//         m_VulkanContext = context;
//     }

//     ~RenderingContext()
//     {
//         vkDestroySurfaceKHR(m_VulkanContext->GetInstance(), m_Surface, nullptr);
//     }

// public:
//     std::shared_ptr<VulkanInstance> m_VulkanContext;
//     VkSurfaceKHR m_Surface;
// };