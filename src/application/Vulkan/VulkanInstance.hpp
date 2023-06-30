#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL3/SDL_vulkan.h>

#include <set>
#include <string>
#include <vector>

struct VulkanInstanceCreateInfo
{
    std::string ApplicationName;
    bool EnableValidationLayers;
    std::vector<const char*> Extensions;
    std::vector<const char*> ValidationLayers;


    VulkanInstanceCreateInfo()
    :   ApplicationName("Default application name"),
        EnableValidationLayers(true)
    {}
};

class VulkanInstance
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

    static std::vector<VkExtensionProperties> EnumerateExtensions();
    static std::vector<VkLayerProperties> EnumerateLayers();
private:
    bool CheckExtensionSupport();
    bool CheckLayerSupport();

private:
    VkInstance m_Instance;
    VulkanInstanceCreateInfo m_CreateInfo;

    std::set<const char*> Extensions;
    std::set<const char*> EnabledExtensions;
    std::set<const char*> EnabledLayers;
};