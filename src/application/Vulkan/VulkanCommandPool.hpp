#pragma once

#include "VulkanDevice.hpp"

class VulkanCommandBuffer;

class VulkanCommandPool : public std::enable_shared_from_this<VulkanCommandPool>
{
public:
    VulkanCommandPool(std::shared_ptr<VulkanDevice> device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
    ~VulkanCommandPool();

    std::unique_ptr<VulkanCommandBuffer> CreatePrimaryBuffer();
    std::vector<std::unique_ptr<VulkanCommandBuffer>> CreatePrimaryBuffers(uint32_t bufferCount);

    void DestroyCommandBuffer(std::unique_ptr<VulkanCommandBuffer> commandBuffer);
    void DestroyCommandBuffers(std::vector<std::unique_ptr<VulkanCommandBuffer>>& commandBuffers);
    
    VkCommandPool GetHandle() const;
    std::shared_ptr<VulkanDevice> GetDevice() const;

private:
    std::shared_ptr<VulkanDevice> m_Device;
    VkCommandPool m_CommandPool;
    uint32_t m_QueueFamilyIndex;
};