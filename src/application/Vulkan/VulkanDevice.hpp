#pragma once

#include "VulkanDeviceSelector.hpp"

#include <map>

struct QueueFamilyInfo
{
    QueueFamilyInfo() : QueueCount(0), QueuePriorities() {}

    int QueueCount;
    std::vector<float> QueuePriorities;
};

class VulkanQueue;

class VulkanDevice : public std::enable_shared_from_this<VulkanDevice>
{
public:
    VulkanDevice() = delete;
    VulkanDevice(const VulkanDevice&) = delete;
    
    VulkanDevice(std::shared_ptr<VulkanPhysicalDevice> physicalDevice, std::shared_ptr<VulkanDeviceRequirements> requirements);
    ~VulkanDevice();
    
    VkDevice GetHandle() { return m_Device; }
    
    const SwapchainSupportDetails GetSwapchainSupportDetails(VkSurfaceKHR surface);
    
    std::shared_ptr<VulkanQueue> GetQueue(const VulkanQueueRequest& request, int queueIndex);

    bool WaitIdle() const;

private:
    std::vector<VkDeviceQueueCreateInfo> GenerateCreateInfos(std::map<uint32_t, QueueFamilyCreateInfo>& familyInfos);

private:
    std::shared_ptr<VulkanInstance> m_Instance;
    std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
    VkDevice m_Device;
};