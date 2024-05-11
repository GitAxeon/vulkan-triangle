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

bool VulkanCommandBuffer::Begin(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags;
    beginInfo.pInheritanceInfo = nullptr; // Optional

    VkResult commandBeginInfoResult = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
    
    return commandBeginInfoResult == VK_SUCCESS;
}

void VulkanCommandBuffer::End()
{
    VkResult result = vkEndCommandBuffer(m_CommandBuffer);

    if(result != VK_SUCCESS)
    {
        Log.Error("Error in VulkanCommandBuffer ",  static_cast<int>(result));
    }
}

void VulkanCommandBuffer::BindPipeline(const VulkanPipeline& pipeline, VkPipelineBindPoint bindPoint)
{
    vkCmdBindPipeline(m_CommandBuffer, bindPoint, pipeline.GetHandle());
}

void VulkanCommandBuffer::Reset(VkCommandBufferResetFlags flags)
{
    VkResult result = vkResetCommandBuffer(m_CommandBuffer, 0);

    if(result != VK_SUCCESS)
    {
        Log.Error("CommandBuffer::Reset failed");
    }
}

void VulkanCommandBuffer::BeginRenderPass
(
    const VulkanRenderPass& renderPass,
    const VulkanFramebuffer& frameBuffer,
    VulkanRect2D renderArea,
    const VkClearValue& clearValue,
    VkSubpassContents subPassContents
)
{
    VkRenderPassBeginInfo renderPassBeginInfo {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass.GetHandle();
    renderPassBeginInfo.framebuffer = frameBuffer.GetHandle();
    renderPassBeginInfo.renderArea.offset = renderArea.offset;
    renderPassBeginInfo.renderArea.extent = renderArea.extent;

    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(m_CommandBuffer, &renderPassBeginInfo, subPassContents);
}

void VulkanCommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(m_CommandBuffer);
}

void VulkanCommandBuffer::SetViewport(const VulkanViewport& viewport)
{
    vkCmdSetViewport(m_CommandBuffer, 0, 1, viewport);
}


void VulkanCommandBuffer::SetScissor(const VulkanRect2D& scissor)
{
    vkCmdSetScissor(m_CommandBuffer, 0, 1, scissor);
}

void VulkanCommandBuffer::Draw(uint32_t vertexCount, uint32_t firstVertex)
{
    vkCmdDraw(m_CommandBuffer, vertexCount, 1, firstVertex, 0);
}