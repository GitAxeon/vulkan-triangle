#pragma once

#include "VulkanPhysicalDevice.hpp"

#include <Vulkan/vulkan.hpp>

class VulkanDeviceSelector
{
public:
    VulkanDeviceSelector(std::shared_ptr<VulkanInstance> instance, std::vector<VulkanQueueRequest>& queueRequests)
        : m_Instance(instance), m_Requests(queueRequests)
    {
    }

    std::shared_ptr<VulkanPhysicalDevice> SelectDevice()
    {
        VkPhysicalDevice result = VK_NULL_HANDLE;

        for(auto device : VulkanPhysicalDevice::EnumeratePhysicalDevices(m_Instance))
        {
            DeviceQueueIndices indices = FindIndices(device);

            if(indices.Valid(m_Requests))
            {
                Log.Info("Suitable device found");
                return std::make_shared<VulkanPhysicalDevice>(m_Instance, device, indices);
            }
        }

        Log.Error("Queue requests couldn't be fulfilled");
        throw std::runtime_error("Failed to find suitable physical device");
    }

private:
    DeviceQueueIndices FindIndices(VkPhysicalDevice device)
    {
        DeviceQueueIndices queueIndices;

        for(auto request : m_Requests)
        {
            int familyIndex = 0;
            for(auto queueFamily : VulkanPhysicalDevice::EnumerateDeviceQueueFamilyProperties(device))
            {
                Log.Info("Queue family queue count: ", queueFamily.queueCount);
                Log.Info("{ ",
                    (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) ? "Graphics " :"",
                    (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) ? "Transfer " : "",
                    (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) ? "Compute " : "",
                    (queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) ? "Sparse " : "",
                " }");
                
                // Doesn't support required operations
                if((queueFamily.queueFlags & request.Flags) != request.Flags)
                    continue;
                
                // Doesn't support present operations
                if(request.Surface.has_value())
                {
                    if(!CheckSurfaceSupport(device, familyIndex, request.Surface.value()))
                        continue;
                }
                    
                queueIndices.Indices.insert(std::make_pair(request.Flags, familyIndex));
                break;
                
                ++familyIndex;
            }
        }

        return queueIndices;
    }

    bool CheckSurfaceSupport(VkPhysicalDevice device, uint32_t familyIndex, VkSurfaceKHR surface)
    {
        VkBool32 surfaceSupport = false;
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(device, familyIndex, surface, &surfaceSupport);
        
        if(result != VK_SUCCESS)
        {
            Log.Error("Failed to query device surface support");
            throw std::runtime_error("Vulkan did an oopsie");
        }

        return surfaceSupport;
    }

private:
    std::shared_ptr<VulkanInstance> m_Instance;
    std::vector<VulkanQueueRequest> m_Requests;
};