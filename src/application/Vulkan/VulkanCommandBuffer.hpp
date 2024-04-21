#pragma once

#include "VulkanCommandPool.hpp"

class VulkanCommandBuffer
{
public:
    VulkanCommandBuffer(std::shared_ptr<VulkanCommandPool> commandPool, VkCommandBuffer handle);
    ~VulkanCommandBuffer();

    VkCommandBuffer GetHandle() const;
    const VkCommandBuffer& GetHandleAddress() const { return m_CommandBuffer; }

    void Reset(VkCommandBufferResetFlags flags = 0);

private:
    friend class VulkanCommandPool;

    std::shared_ptr<VulkanCommandPool> m_CommandPool;
    VkCommandBuffer m_CommandBuffer;
};