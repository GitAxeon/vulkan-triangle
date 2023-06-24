#pragma once

#include "../debug/Log.hpp"
#include "VulkanInstance.hpp"

#include <vulkan/vulkan.hpp>
#include <SDL3/SDL_vulkan.h>

#include <optional>

struct QueueFamilyIndices
{
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentFamily;

    bool IsComplete()
    {
        return GraphicsFamily.has_value() && PresentFamily.has_value();
    }
};

class VulkanPhysicalDevice
{
public:
    VulkanPhysicalDevice(std::shared_ptr<VulkanInstance> vulkanInstance)
    {
        m_PhysicalDevice = VK_NULL_HANDLE;

        for(const auto& device : EnumeratePhysicalDevices(vulkanInstance))
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
            throw std::runtime_error("No suitable GraphicsDevice found");
        }
    }

    static std::vector<VkPhysicalDevice> EnumeratePhysicalDevices(std::shared_ptr<VulkanInstance> vulkanInstance)
    {
        uint32_t deviceCount = 0;
        VkResult operationResult = vkEnumeratePhysicalDevices(vulkanInstance->GetInstance(), &deviceCount, nullptr);

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
        vkEnumeratePhysicalDevices(vulkanInstance->GetInstance(), &deviceCount, devices.data());

        if(operationResult != VK_SUCCESS)
        {
            Log.Error("vkEnumeratePhysicalDevices failed listing devices VkResult", operationResult);
        }

        return devices;
    }

    std::vector<VkQueueFamilyProperties> EnumerateDeviceQueueFamilyProperties(VkPhysicalDevice device)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        
        return queueFamilies;
    }

    bool IsSuitableDevice(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device);

        return indices.IsComplete();
    }

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;
        
        auto queueFamilies = EnumerateDeviceQueueFamilyProperties(device);

        int i = 0;
        for(const auto& queueFamily : queueFamilies)
        {
            if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.GraphicsFamily = i;
            }

            // VkBool32 presentSupport = false;
            // vkGetPhysicalDeviceSurfaceSupportKHR(device, i, )

            if(indices.IsComplete())
                break;

            ++i;
        }

        return indices;
    }

private:
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
};

// uint32_t CreateLogicalDevice()
// {
//     QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

//     VkDeviceQueueCreateInfo queueCreateInfo{};
//     queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//     queueCreateInfo.queueFamilyIndex = indices.GraphicsFamily.value();
//     queueCreateInfo.queueCount = 1;

//     float queuePriority = 1.0f;
//     queueCreateInfo.pQueuePriorities = &queuePriority;

//     VkPhysicalDeviceFeatures deviceFeatures{};

//     VkDeviceCreateInfo createInfo{};
//     createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//     createInfo.pQueueCreateInfos = &queueCreateInfo;
//     createInfo.queueCreateInfoCount = 1;
//     createInfo.pEnabledFeatures = &deviceFeatures;

//     createInfo.enabledExtensionCount = 0;
    
//     if(m_EnableValidationLayers)
//     {
//         createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
//         createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
//     }
//     else
//     {
//         createInfo.enabledLayerCount = 0;
//     }

//     VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);

//     if(result != VK_SUCCESS)
//     {
//         return VK_ERROR_INITIALIZATION_FAILED;
//     }

//     vkGetDeviceQueue(m_Device, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);

//     return VK_SUCCESS;
// }