#pragma once

#include "VulkanDeviceSelector.hpp"

#include <map>

struct QueueFamilyInfo
{
    QueueFamilyInfo() : QueueCount(0), QueuePriorities() {}

    int QueueCount;
    std::vector<float> QueuePriorities;
};

class VulkanDevice
{
public:
    VulkanDevice() = delete;
    VulkanDevice(const VulkanDevice&) = delete;
    
    VulkanDevice(std::shared_ptr<VulkanPhysicalDevice> physicalDevice, VulkanDeviceRequirements& requirements);
    const SwapchainSupportDetails GetSwapchainSupportDetails(VkSurfaceKHR surface);

    ~VulkanDevice();

    VkQueue GetQueue(const VulkanQueueRequest& request, int queueIndex);

private:
    std::vector<VkDeviceQueueCreateInfo> GenerateCreateInfos(std::map<uint32_t, QueueFamilyCreateInfo>& familyInfos);

private:
    std::shared_ptr<VulkanInstance> m_Instance;
    std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
    VkDevice m_Device;
};