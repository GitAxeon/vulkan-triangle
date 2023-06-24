#include "VulkanInstance.hpp"
#include "../debug/Log.hpp"

VulkanInstance::VulkanInstance(const VulkanInstanceCreateInfo& instanceCreateInfo)
    : m_Instance(nullptr), m_CreateInfo(instanceCreateInfo)
{        
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = instanceCreateInfo.ApplicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    auto properties = EnumerateExtensions();
    Log.Info("Available Vulkan extensions [", properties.size(), "]:");
    size_t i = 0;
    for(auto props : properties)
    {
        Extensions.insert(props.extensionName);
        Log.Info("    ",i++, ": ", props.extensionName);
    }

    if(!CheckExtensionSupport())
    {
        Log.Error("One or more requested extensions unavailable");
        throw std::runtime_error("Requested Vulkan extensions not available");
    }

    if(!CheckLayerSupport())
    {
        Log.Error("One or more requested layers unavailable");
        throw std::runtime_error("Requested Vulkan validation layer not available");
    }

    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    createInfo.enabledExtensionCount = instanceCreateInfo.Extensions.size();
    createInfo.ppEnabledExtensionNames = instanceCreateInfo.Extensions.data();

    if(instanceCreateInfo.EnableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(instanceCreateInfo.ValidationLayers.size());
        createInfo.ppEnabledLayerNames = instanceCreateInfo.ValidationLayers.data();
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

    for(auto ext : instanceCreateInfo.Extensions)
        EnabledExtensions.insert(ext);

    for(auto ext : instanceCreateInfo.ValidationLayers)
        EnabledLayers.insert(ext);

    Log.Info("Created Vulkan instance");
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

std::vector<VkExtensionProperties> VulkanInstance::EnumerateExtensions()
{
    uint32_t count = 0;
    VkResult enumerateResult = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

    if(enumerateResult != VK_SUCCESS)
    {
        Log.Error("Failed to enumerate Vulkan extensions");
    }

    if(count == 0)
    {
        Log.Error("No Vulkan extensions available VkResult", enumerateResult);
    }

    std::vector<VkExtensionProperties> properties;
    properties.resize(count);

    enumerateResult = vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data());

    if(enumerateResult != VK_SUCCESS)
    {
        Log.Error("Failed to enumerate Vulkan extensions");
    }

    return properties;
}

std::vector<VkLayerProperties> VulkanInstance::EnumerateLayers()
{
    uint32_t count = 0;
    VkResult enumerateResult = vkEnumerateInstanceLayerProperties(&count, nullptr);

    if(enumerateResult != VK_SUCCESS)
    {
        Log.Error("Failed to enumerate Vulkan layers");
    }

    if(count == 0)
    {
        Log.Error("Vulkan validation layers not available VkResult", enumerateResult);
    }

    std::vector<VkLayerProperties> properties;
    properties.resize(count);

    enumerateResult = vkEnumerateInstanceLayerProperties(&count, properties.data());

    if(enumerateResult != VK_SUCCESS)
    {
        Log.Error("Failed to enumerate Vulkan layers");
    }

    return properties;
}

bool VulkanInstance::CheckExtensionSupport()
{
    size_t total = m_CreateInfo.Extensions.size();
    Log.Info("Checking extension support [", total, "]");

    size_t foundExtensionsCount = 0;
    for(auto ext : m_CreateInfo.Extensions)
    {
        bool found = false;

        for(auto props : EnumerateExtensions())
        {
            if(strcmp(ext, props.extensionName) == 0)
            {
                found = true;
                ++foundExtensionsCount;
                Log.Info("    \"", ext, "\" - Found");
                break;
            }
        }

        if(!found)
        {
            Log.Error("    \"", ext, "\" - Not found");
        }
    }

    Log.Info("- OK [", foundExtensionsCount, "]", " - FAILED [", total - foundExtensionsCount, "]");
    
    if(foundExtensionsCount != m_CreateInfo.Extensions.size())
    {
        return false;
    }

    return true;
}

bool VulkanInstance::CheckLayerSupport()
{
    size_t total = m_CreateInfo.ValidationLayers.size();
    Log.Info("Checking validation layer support [", total, "]");

    size_t foundLayerCount = 0;
    for(auto ext : m_CreateInfo.ValidationLayers)
    {
        bool found = false;

        for(auto props : EnumerateLayers())
        {
            if(strcmp(ext, props.layerName) == 0)
            {
                found = true;
                ++foundLayerCount;
                Log.Info("    \"", ext, "\" - Found");
                break;
            }
        }

        if(!found)
        {
            Log.Error("    \"", ext, "\" - Not found");
        }
    }

    Log.Info("- OK [", foundLayerCount, "]", " - FAILED [", total - foundLayerCount, "]");

    if(foundLayerCount != total)
    {
        return false;
    }

    return true;
}