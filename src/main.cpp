#include "Log.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>
#include <glm/mat2x2.hpp>

#include <array>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

static LoggerBase<std::ostream> Log(std::cout);

struct QueueFamilyIndices
{
    std::optional<uint32_t> GraphicsFamily;
};

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
    
    ~VulkanContext()
    {
        if(m_EnableValidationLayers)
        {
            Log.Info("Destructed debug messenger");
            DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
        }

        Log.Info("Destructed VulkanContext");
        vkDestroyInstance(m_Instance, nullptr);
    }

    uint32_t Init(const std::string& applicationName)
    {
        if(m_EnableValidationLayers && !CheckValidationLayerSupport())
        {
            Log.Error("Validation layers requested but not available");
            return VK_ERROR_LAYER_NOT_PRESENT; 
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = applicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        std::vector<const char*> SDLExtensions;
        VkResult extensionResult = GetInstanceExtensions(SDLExtensions);

        if(extensionResult != VK_SUCCESS)
        {
            return extensionResult;
        }

        VkInstanceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = SDLExtensions.size();
        createInfo.ppEnabledExtensionNames = SDLExtensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if(m_EnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
            createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

            FillDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
        }

        VkResult instanceResult = vkCreateInstance(&createInfo, nullptr, &m_Instance);

        if(instanceResult != VK_SUCCESS)
        {
            Log.Error("Failed to create Vulkan instance ", "VkResult: ", instanceResult);
            return instanceResult;
        }

        Log.Info("Created Vulkan instance");

        if(SetupDebugMessenger())
        {
            Log.Info("Created Vulkan debug messenger");
        }
        else
        {
            Log.Warn("Debugging not enabled");
        }

        SelectPhysicalDevice();

        
        return instanceResult;
    }

    void EnableValidationLayers()
    {
        m_EnableValidationLayers = true;
    }

private:
    VkResult GetInstanceExtensions(std::vector<const char*>& extensions)
    {
        uint32_t SDLExtensionCount = 0;
        SDL_bool extensionResult = SDL_Vulkan_GetInstanceExtensions(&SDLExtensionCount, nullptr);

        if(extensionResult == SDL_FALSE)
        {
            Log.Error("Failed to retrieve necessary Vulkan extensions");
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
        else
        {
            Log.Info("Retrieved ", SDLExtensionCount, " extensions");
        }

        extensions.resize(SDLExtensionCount);

        extensionResult = SDL_Vulkan_GetInstanceExtensions(&SDLExtensionCount, extensions.data()); 
        
        if(extensionResult == SDL_FALSE)
        {
            Log.Error("Failed to retrieve necessary Vulkan extensions");
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        if(m_EnableValidationLayers)
        {
            Log.Info("Included debug messenger extension");
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        
        for(size_t i = 0; i < extensions.size(); i++)
        {
            Log.Info(i + 1, ": ", extensions[i]);
        }

        return VK_SUCCESS;
    }

    bool CheckValidationLayerSupport() const
    {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for(auto layerName : m_ValidationLayers)
        {
            bool layerFound = false;
            
            for(const auto& layerProperties : availableLayers)
            {
                if(strcmp(layerName, layerProperties.layerName) == 0)
                {
                    Log.Info("Found validation layer \"", layerName, "\"");
                    layerFound = true;
                    break;
                }
            }

            if(!layerFound)
            {
                Log.Error("Couldn't find validation layer \"", layerName, "\"");
                return false;
            }
        }

        return true;
    }

    void FillDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& info)
    {
        info = {};
        info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        info.pfnUserCallback = DebugCallback;
        info.pUserData = nullptr;
    }
    
    bool SetupDebugMessenger()
    {
        if(!m_EnableValidationLayers) return false;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        FillDebugMessengerCreateInfo(createInfo);

        if(CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
        {
            return false;
        }

        return true;
    }

    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        if(func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

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

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");

        if(func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    uint32_t SelectPhysicalDevice()
    {
        m_PhysicalDevice = VK_NULL_HANDLE;

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if(deviceCount == 0)
        {
            Log.Info("Failed to find GPUs with Vulkan support!");
            return VK_ERROR_UNKNOWN;
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        for(const auto& device : devices)
        {
            if(IsSuitableDevice(device))
            {
                m_PhysicalDevice = device;
                break;
            }
        }

        if(m_PhysicalDevice == VK_NULL_HANDLE)
        {
            Log.Error("Failed to find a suitable GPU!");
            return VK_ERROR_UNKNOWN;
        }
    }

    bool IsSuitableDevice(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;

        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
    }

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;
        
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for(const auto& queueFamily : queueFamilies)
        {
            if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.GraphicsFamily = i;
            }

            ++i;
        }

        return indices;
    }

private:
    VkInstance m_Instance;
    VkPhysicalDevice m_PhysicalDevice;

    bool m_EnableValidationLayers = false;
    VkDebugUtilsMessengerEXT m_DebugMessenger;

    std::array<const char*, 1> m_ValidationLayers = 
    {
        "VK_LAYER_KHRONOS_validation"
    };
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

    VulkanContext context;
    context.EnableValidationLayers();
    
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