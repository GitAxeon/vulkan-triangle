#include "VulkanCommandBuffer.hpp"

VulkanCommandBuffer::VulkanCommandBuffer(std::shared_ptr<VulkanCommandPool> commandPool, VkCommandBuffer handle)
    : m_CommandPool(commandPool), m_CommandBuffer(handle)
{
    Log.Info("VulkanCommandBuffer instantiated");
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    Log.Info("VulkanCommandBuffer retired");
}

VkCommandBuffer VulkanCommandBuffer::GetHandle() const
{
    return m_CommandBuffer;
}

void VulkanCommandBuffer::Reset(VkCommandBufferResetFlags flags)
{
    vkResetCommandBuffer(m_CommandBuffer, 0);
}