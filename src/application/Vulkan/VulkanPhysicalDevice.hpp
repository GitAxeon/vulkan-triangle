#pragma once

#include "../debug/Log.hpp"
#include "VulkanInstance.hpp"

#include <vulkan/vulkan.hpp>
#include <SDL3/SDL_vulkan.h>

#include <optional>
#include <map>

struct VulkanQueueRequest
{
    VkQueueFlagBits Flags;
    std::optional<VkSurfaceKHR> Surface;
    float Priority;
    uint32_t Count;
};

struct DeviceQueueIndices
{
    std::map<VkQueueFlagBits, uint32_t> Indices;

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


// Idk the thing below this could be done by "device selector"?
private:
    // bool IsSuitableDevice(VkPhysicalDevice device, const VulkanDeviceRequirements& requirements)
    // {
    //     QueueFamilyIndices indices = FindQueueFamilies(device, requirements);

    //     if(indices.IsComplete())
    //     {
    //         m_QueueFamilyIndices = indices;
    //     }

    //     return indices.IsComplete();
    // }
    
    // QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, const VulkanDeviceRequirements& requirements)
    // {
    //     QueueFamilyIndices indices;
        
    //     auto queueFamilies = EnumerateDeviceQueueFamilyProperties(device);

    //     Log.Info("Listing queue families");

    //     int i = 0;
    //     for(const auto& queueFamily : queueFamilies)
    //     {
    //         // Log.Info("Queue family queue count: ", queueFamily.queueCount);
    //         // Log.Info("{ ",
    //         //     (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) ? "Graphics " :"",
    //         //     (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) ? "Transfer " : "",
    //         //     (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) ? "Compute " : "",
    //         //     (queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) ? "Sparse " : "",
    //         // " }");
            
    //         if((queueFamily.queueFlags & requirements.QueueFlags) == requirements.QueueFlags)
    //         {
    //             // Log.Info("Flag bits matching");
    //             indices.GraphicsFamily = i;
    //         }

    //         if(requirements.Surface.has_value())
    //         {
    //             VkBool32 presentSupport = false;
    //             vkGetPhysicalDeviceSurfaceSupportKHR(device, i, requirements.Surface.value(), &presentSupport);

    //             if(presentSupport)
    //                 indices.PresentFamily = i;
    //         }

    //         if(indices.IsComplete())
    //             break;

    //         ++i;
    //     }

    //     return indices;
    // }

private:
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    std::shared_ptr<VulkanInstance> m_Instance;
    DeviceQueueIndices m_QueueFamilyIndices;
};