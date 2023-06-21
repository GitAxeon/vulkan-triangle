#include "VulkanInstance.hpp"
#include "../Log.hpp"

VulkanInstance::VulkanInstance(const VulkanInstanceCreateInfo& createInfo)
    : m_Instance(nullptr), m_CreateInfo(createInfo)
{        
    if(!CheckValidationLayerSupport())
        throw std::runtime_error("Validation layers not available");

    CreateInstance();
}

VulkanInstance::~VulkanInstance()
{
    Log.Info("Destructed VulkanInstance");
    vkDestroyInstance(m_Instance, nullptr);
}

VkInstance VulkanInstance::GetInstance() const
{ 
    return m_Instance;
}

void VulkanInstance::CreateInstance()
{
    if(SDL_Vulkan_LoadLibrary(nullptr) != 0)
    {
        Log.Error("Failed to load Vulkan");
        throw std::runtime_error("Failed to load Vulkan");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = m_CreateInfo.ApplicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    std::vector<const char*> SDLExtensions;
    VkResult extensionResult = GetInstanceExtensions(SDLExtensions);

    if(extensionResult != VK_SUCCESS)
    {
        Log.Error("Failed to retrieve required Vulkan extensions");
        throw std::runtime_error("Failed to retrieve required Vulkan extensions");
    }

    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = SDLExtensions.size();
    createInfo.ppEnabledExtensionNames = SDLExtensions.data();

    if(m_CreateInfo.EnableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_CreateInfo.ValidationLayers.size());
        createInfo.ppEnabledLayerNames = m_CreateInfo.ValidationLayers.data();
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
        throw std::runtime_error("Vulkan error");
    }

    Log.Info("Created Vulkan instance");
}

VkResult VulkanInstance::GetInstanceExtensions(std::vector<const char*>& extensions)
{
    uint32_t SDLExtensionCount = 0;
    SDL_bool extensionResult = SDL_Vulkan_GetInstanceExtensions(&SDLExtensionCount, nullptr);

    if(extensionResult == SDL_FALSE)
    {
        Log.Error("Failed to retrieve necessary Vulkan extensions");
        Log.Error("SDL Error:" , SDL_GetError());
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
        Log.Error("SDL Error:" , SDL_GetError());
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    if(m_CreateInfo.EnableValidationLayers)
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

bool VulkanInstance::CheckValidationLayerSupport() const
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for(auto layerName : m_CreateInfo.ValidationLayers)
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