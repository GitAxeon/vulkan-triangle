#pragma once

#include "../debug/Log.hpp"
#include "VulkanInstance.hpp"

#include <vulkan/vulkan.hpp>
#include <SDL3/SDL_vulkan.h>

#include <optional>
#include <map>

struct QueueLocation
{
    // Index of the queue family
    uint32_t FamilyIndex;

    // Queue index within the family
    uint32_t Index;
};
    
struct VulkanQueueRequest
{
    VkQueueFlags Flags;
    std::optional<VkSurfaceKHR> Surface;
    uint32_t Count;
    std::vector<float> Priorities;

    // Gets filled with locations to requested queues within the physical device
    std::vector<QueueLocation> QueueLocations;
};

struct VulkanQueueFamilyInfo
{
    VkQueueFamilyProperties Properties;
    bool PresentSupport = false;
    uint32_t Index;
};

struct VulkanDeviceInfo
{
    VkPhysicalDeviceProperties Properties;
    VkPhysicalDeviceFeatures Features;

    std::vector<VkExtensionProperties> ExtensionProperties;
    std::vector<std::string> Extensions;
    std::vector<std::string> EnabledExtensions;
    
    std::vector<VulkanQueueFamilyInfo> QueueFamilies;
    uint32_t Index;
};

class VulkanPhysicalDevice
{
public:
    VulkanPhysicalDevice(std::shared_ptr<VulkanInstance> vulkanInstance, VkPhysicalDevice deviceHandle, uint32_t deviceId)
        : m_Instance(vulkanInstance), m_PhysicalDevice(deviceHandle), m_PhysicalDeviceIndex(deviceId)
    {
        QueryDeviceProperties();
        QueryDeviceFeatures();
        QueryDeviceExtensionProperties();
        
        // m_Extension contains only the names of available extensions
        for(auto& extension : m_ExtensionProperties)
        {
            m_Extensions.push_back(extension.extensionName);
        }

        QueryDeviceQueueFamilyInfos();
    }

    void QueryDeviceProperties()
    {
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);
    };

    const VkPhysicalDeviceProperties& GetProperties() { return m_Properties;}

    void QueryDeviceFeatures()
    {
        vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);
    }

    const VkPhysicalDeviceFeatures& GetFeatures() { return m_Features; }

    void QueryDeviceExtensionProperties()
    {
        uint32_t extensionCount;
        VkResult operationResult;

        operationResult= vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);

        if(operationResult != VK_SUCCESS)
        {
            Log.Error("vkEnumerateDeviceExtensionProperties #1 call failed");
            throw std::runtime_error("Failed to query physical device extension properties");
        }

        // Resize the vector to accommodate the extensions
        m_ExtensionProperties.resize(extensionCount);

        operationResult = vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, m_ExtensionProperties.data());

        if(operationResult != VK_SUCCESS)
        {
            Log.Error("vkEnumerateDeviceExtensionProperties #2 call failed");
            throw std::runtime_error("Failed to query physical device extension properties");
        }
    }

    const std::vector<VkExtensionProperties>& GetExtensionProperties() { return m_ExtensionProperties; }
    const std::vector<std::string>& GetExtensions() { return m_Extensions; }
    const std::vector<std::string>& GetEnabledExtensions() { return m_EnabledExtensions; }

    void QueryDeviceQueueFamilyInfos()
    {
        uint32_t familyIndex = 0;
        for(auto familyProperties : EnumerateDeviceQueueFamilyProperties())
        {
            VulkanQueueFamilyInfo familyInfo;
            familyInfo.Properties = familyProperties;
            familyInfo.Index = familyIndex;
            familyInfo.PresentSupport = false;

            m_QueueFamilies.push_back(familyInfo);

            familyIndex++;
        }
    }

    const std::vector<VulkanQueueFamilyInfo>& GetQueueFamilyInfos() { return m_QueueFamilies; }


    VulkanPhysicalDevice(std::shared_ptr<VulkanInstance> vulkanInstance, VkPhysicalDevice device)
        : m_Instance(vulkanInstance),  m_PhysicalDevice(device)
    {
        Log.Info("Created VulkanPhysicalDevice");
    }
    
    ~VulkanPhysicalDevice()
    {
        Log.Info("Destructed VulkanPhysicalDevice");
    }

    VkPhysicalDevice GetHandle() const 
    {
        return m_PhysicalDevice;
    }

    static std::vector<VkPhysicalDevice> EnumeratePhysicalDevices(std::shared_ptr<VulkanInstance> vulkanInstance)
    {
        uint32_t deviceCount = 0;
        VkResult operationResult;
        
        operationResult = vkEnumeratePhysicalDevices(vulkanInstance->GetInstance(), &deviceCount, nullptr);

        if(operationResult != VK_SUCCESS)
        {
            Log.Error("vkEnumeratePhysicalDevices failed at getting deviceCount VkResult", operationResult);
        }

        if(deviceCount == 0)
        {
            Log.Info("Failed to find GPUs with Vulkan support!");
            throw std::runtime_error("No suitable GraphicsDevice found");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        operationResult = vkEnumeratePhysicalDevices(vulkanInstance->GetInstance(), &deviceCount, devices.data());

        if(operationResult != VK_SUCCESS)
        {
            Log.Error("vkEnumeratePhysicalDevices failed listing devices VkResult", operationResult);
        }

        return devices;
    }

    static VkPhysicalDevice GetDevice(std::shared_ptr<VulkanInstance> instance, int id)
    {
        auto devices = EnumeratePhysicalDevices(instance);
        
        assert((id > -1) && (id < devices.size()));

        return devices[id];
    }

    VkPhysicalDeviceProperties GetDeviceProperties()
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);
        return properties;
    }

    VkPhysicalDeviceFeatures GetDeviceFeatures() 
    {
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &features);
        return features;
    }

    std::vector<VkQueueFamilyProperties> EnumerateDeviceQueueFamilyProperties()
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilies.data());
        
        return queueFamilies;
    }

    std::vector<VkExtensionProperties> GetDeviceExtensions()
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> properties(extensionCount);
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, properties.data());

        return properties;
    };

    static VkPhysicalDeviceProperties GetDeviceProperties(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        return properties;
    }

    static VkPhysicalDeviceFeatures GetDeviceFeatures(VkPhysicalDevice device) 
    {
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);
        return features;
    }

    static std::vector<VkQueueFamilyProperties> EnumerateDeviceQueueFamilyProperties(VkPhysicalDevice device)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        
        return queueFamilies;
    }

    static std::vector<VkExtensionProperties> GetDeviceExtensions(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        
        std::vector<VkExtensionProperties> properties(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, properties.data());

        return properties;
    };

private:
    std::shared_ptr<VulkanInstance> m_Instance;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

    VkPhysicalDeviceProperties m_Properties;
    VkPhysicalDeviceFeatures m_Features;

    std::vector<VkExtensionProperties> m_ExtensionProperties;

    // Contains only the names of available extensions
    std::vector<std::string> m_Extensions;

    // Contains names of enabled extensions. This vector gets filled by the DeviceSelector
    std::vector<std::string> m_EnabledExtensions;
    
    // Contains information of each QueueFamily
    std::vector<VulkanQueueFamilyInfo> m_QueueFamilies;

    uint32_t m_PhysicalDeviceIndex;
};