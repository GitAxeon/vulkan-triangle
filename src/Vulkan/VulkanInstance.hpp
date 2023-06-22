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

    static std::vector<VkExtensionProperties> EnumerateExtensions();
    static std::vector<VkLayerProperties> EnumerateLayers();
private:
    bool CheckExtensionSupport();
    bool CheckLayerSupport();

private:
    VkInstance m_Instance;
    VulkanInstanceCreateInfo m_CreateInfo;

    std::set<std::string> Extensions;
    std::set<std::string> EnabledExtensions;
    std::set<std::string> EnabledLayers;
};