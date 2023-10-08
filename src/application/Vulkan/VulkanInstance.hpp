#pragma once

#include "VulkanPhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>
#include <SDL3/SDL_vulkan.h>

#include <set>
#include <string>
#include <vector>
#include <memory>

struct VulkanInstanceCreateInfo
{
    std::string ApplicationName;
    bool EnableValidationLayers;
    std::vector<const char*> Extensions;
    std::vector<const char*> ValidationLayers;

    VulkanInstanceCreateInfo()
    :   ApplicationName("Default application name"),
        EnableValidationLayers(true),
        Extensions({}),
        ValidationLayers({})
    {}
};

class VulkanInstance : public std::enable_shared_from_this<VulkanInstance>
{
public:
    VulkanInstance(const VulkanInstanceCreateInfo& createInfo);
    ~VulkanInstance();

    VkInstance GetInstance() const;

    bool Debugging() const 
    {
        return m_CreateInfo.EnableValidationLayers;
    }

    std::vector<const char*> GetEnabledLayers()
    {
        return std::vector<const char*>(EnabledLayers.begin(), EnabledLayers.end());
    }

    std::vector<std::shared_ptr<VulkanPhysicalDevice>> GetPhysicalDevices();

    static std::vector<VkExtensionProperties> EnumerateExtensions();
    static std::vector<VkLayerProperties> EnumerateLayers();
private:
    bool CheckExtensionSupport();
    bool CheckLayerSupport();

private:
    VkInstance m_Instance;
    VulkanInstanceCreateInfo m_CreateInfo;

    // List of all supported extensions
    std::set<const char*> Extensions;

    // List of enabled extensions
    std::set<const char*> EnabledExtensions;
    
    // List of enabled validation layers
    std::set<const char*> EnabledLayers;
};