#pragma once

#include "VulkanCommandPool.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanRect2D.hpp"
#include "VulkanViewport.hpp"
#include "VulkanPipeline.hpp"

class VulkanCommandBuffer
{
public:
    VulkanCommandBuffer(std::shared_ptr<VulkanCommandPool> commandPool, VkCommandBuffer handle);
    ~VulkanCommandBuffer();

    VkCommandBuffer GetHandle() const;
    const VkCommandBuffer& GetHandleAddress() const { return m_CommandBuffer; }
    
    bool Begin(VkCommandBufferUsageFlags flags = 0);
    void End();
    void BindPipeline(std::shared_ptr<VulkanPipeline> pipeline, VkPipelineBindPoint bindPoint);

    void Reset(VkCommandBufferResetFlags flags = 0);

    void BeginRenderPass(
        std::shared_ptr<VulkanRenderPass> renderPass,
        std::shared_ptr<VulkanFramebuffer> frameBuffer, 
        VulkanRect2D renderArea,
        const VkClearValue& clearValue,
        VkSubpassContents subPassContents
    );

    void EndRenderPass();
    void SetViewport(const VulkanViewport& viewport);
    void SetScissor(const VulkanRect2D& scissor);

    void Draw(uint32_t vertexCount, uint32_t firstVertex = 0);

private:
    friend class VulkanCommandPool;

    std::shared_ptr<VulkanCommandPool> m_CommandPool;
    VkCommandBuffer m_CommandBuffer;
};