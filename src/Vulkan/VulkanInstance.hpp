#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL3/SDL_vulkan.h>

#include <set>
#include <string>

struct VulkanInstanceCreateInfo
{
    std::string ApplicationName;
    bool EnableValidationLayers;
    std::vector<const char*> Extensions;
    std::vector<const char*> ValidationLayers;


    VulkanInstanceCreateInfo()
    :   ApplicationName("Default application name"),
        EnableValidationLayers(true),
        ValidationLayers({"VK_LAYER_KHRONOS_validation"})
    {}
};

class VulkanInstance
{
public:

    VulkanInstance(const VulkanInstanceCreateInfo& createInfo);
    ~VulkanInstance();

    VkInstance GetInstance() const;

private:
    void CreateInstance();
    bool CheckValidationLayerSupport() const;
    VkResult GetInstanceExtensions(std::vector<const char*>& extensions);
    bool SetupDebugMessenger();
    void FillDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& info);

private:
    VkInstance m_Instance;
    VulkanInstanceCreateInfo m_CreateInfo;

    std::set<std::string> Extensions;
    std::set<std::string> EnabledExtensions;
    std::set<std::string> EnabledLayers;
};