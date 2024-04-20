#pragma once

#include "VulkanDevice.hpp"

class VulkanFence
{
public:
    VulkanFence(std::shared_ptr<VulkanDevice> device, VkFenceCreateFlags flags = 0);
    ~VulkanFence();

    VkResult Wait(uint64_t timeout = std::numeric_limits<uint64_t>::max());
    VkResult Reset();

    VkFence GetHandle() const;

private:
    std::shared_ptr<VulkanDevice> m_Device;
    VkFence m_Fence;
};