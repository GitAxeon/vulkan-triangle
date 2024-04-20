#pragma once

#include "VulkanDevice.hpp"

class VulkanSemaphore
{
public:
    VulkanSemaphore(std::shared_ptr<VulkanDevice> device);
    ~VulkanSemaphore();

    VkSemaphore GetHandle() const;

private:
    std::shared_ptr<VulkanDevice> m_Device;
    VkSemaphore m_Semaphore;
};