#pragma once

#include "VulkanPhysicalDevice.hpp"

class VulkanDevice
{
public:
    VulkanDevice(std::shared_ptr<VulkanInstance> instance, std::shared_ptr<VulkanPhysicalDevice> physicalDevice, const std::vector<VulkanQueueRequest>& queueRequests)
        : m_Instance(instance), m_PhysicalDevice(physicalDevice), m_Device(VK_NULL_HANDLE)
    {
        auto queueCreateInfos = GenerateCreateInfos(queueRequests);

        // Using default values for now
        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = 0;
        
        std::vector<const char*> ValidationLayers = instance->GetEnabledLayers();

        if(m_Instance->Debugging())
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
            createInfo.ppEnabledLayerNames = ValidationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        VkResult result = vkCreateDevice(m_PhysicalDevice->GetHandle(), &createInfo, nullptr, &m_Device);

        if(result != VK_SUCCESS)
        {
            Log.Error("Failed to create Vulkan LogicalDevice");
            throw std::runtime_error("Graphics error");
        }

        Log.Info("Created VulkanDevice");
    }

    ~VulkanDevice()
    {
        vkDestroyDevice(m_Device, nullptr);
        Log.Info("Destructed VulkanDevice");
    }

private:
    std::vector<VkDeviceQueueCreateInfo> GenerateCreateInfos(const std::vector<VulkanQueueRequest>& requests)
    {
        std::vector<VkDeviceQueueCreateInfo> result;

        DeviceQueueIndices indices = m_PhysicalDevice->GetQueueFamilyIndices();
        for(auto request : requests)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = indices.Indices.find(request.Flags)->second;
            queueCreateInfo.queueCount = request.Count;

            queueCreateInfo.pQueuePriorities = &request.Priority;
            
            result.emplace_back(queueCreateInfo);
        }

        return result;
    }

private:
    std::shared_ptr<VulkanInstance> m_Instance;
    std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
    VkDevice m_Device;
};