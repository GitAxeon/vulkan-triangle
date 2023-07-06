#pragma once

#include "../debug/Log.hpp"
#include "VulkanInstance.hpp"

#include <vulkan/vulkan.hpp>
#include <SDL3/SDL_vulkan.h>

#include <optional>
#include <map>

// I have misunderstood the Vulkan queue creation so it needs to be fixed later
struct VulkanQueueRequest
{
    VkQueueFlags Flags;
    std::optional<VkSurfaceKHR> Surface;
    uint32_t Count;
    std::vector<float> Priorities;
};

struct DeviceQueueIndices
{
    std::map<VkQueueFlags, uint32_t> Indices;

    bool Valid(std::vector<VulkanQueueRequest>& requests)
    {
        for(auto request : requests)
        {
            if(Indices.find(request.Flags) == Indices.end())
                return false;
        }

        return true;
    }
};

class VulkanPhysicalDevice
{
public:
    VulkanPhysicalDevice(std::shared_ptr<VulkanInstance> vulkanInstance, VkPhysicalDevice device, DeviceQueueIndices indices)
        : m_Instance(vulkanInstance),  m_PhysicalDevice(device), m_QueueFamilyIndices(indices)
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

    DeviceQueueIndices GetQueueFamilyIndices() const
    {
        return m_QueueFamilyIndices;
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

private:
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    std::shared_ptr<VulkanInstance> m_Instance;
    DeviceQueueIndices m_QueueFamilyIndices;
};