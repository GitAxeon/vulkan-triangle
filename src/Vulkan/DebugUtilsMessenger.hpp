#pragma once

#include "../Log.hpp"
#include "VulkanInstance.hpp"

#include <vulkan/vulkan.hpp>

#include <functional>

#define FETCH_VK_FUNCTION(instance, function) \
    reinterpret_cast<PFN_##function>(vkGetInstanceProcAddr(instance, #function))

class DebugUtilsMessenger
{
public:
    DebugUtilsMessenger() = delete;

    DebugUtilsMessenger(
        std::shared_ptr<VulkanInstance> instance,
        PFN_vkDebugUtilsMessengerCallbackEXT fn
    )
        : m_Instance(instance), m_DebugMessenger {}
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo {};

        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = fn;
        createInfo.pUserData = nullptr;

        auto CreateDebugUtilsMessengerEXT = FETCH_VK_FUNCTION(instance->GetInstance(), vkCreateDebugUtilsMessengerEXT);
        
        if(CreateDebugUtilsMessengerEXT == nullptr)
        {
            Log.Error("Failed to fetch vkCreateDebugUtilsMessengerEXT");
            Log.Error("Failed to properly initialize DebugUtilsMessenger");
            // throw std::runtime_error("Failed to fetch vkCreateDebugUtilsMessengerEXT");
        }
        
        VkResult result = CreateDebugUtilsMessengerEXT(m_Instance->GetInstance(), &createInfo, nullptr, &m_DebugMessenger);
        
        if(result != VK_SUCCESS)
        {
            Log.Error("Failed to create VulkanDebugMessenger. VkResult: ", result);
            Log.Error("Failed to properly initialize DebugUtilsMessenger");
            // throw std::runtime_error("Failed to create debug messenger");
        }

        Log.Info("Created VulkanDebugMessenger successfully");
    }

    ~DebugUtilsMessenger()
    {
        auto DestroyDebugUtilsMessengerEXT = FETCH_VK_FUNCTION(m_Instance->GetInstance(), vkDestroyDebugUtilsMessengerEXT);

        if(DestroyDebugUtilsMessengerEXT)
        {
            Log.Info("Destructed VulkanDebugMessenger");
            DestroyDebugUtilsMessengerEXT(m_Instance->GetInstance(), m_DebugMessenger, nullptr);
        }
        else
        {
            Log.Warn("Failed to fetch vkDestroyDebugUtilsMessengerEXT");
            Log.Error("Failed to destroy VkDebugUtilsMessenger");
        }
    }
    

private:
    std::shared_ptr<VulkanInstance> m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
};