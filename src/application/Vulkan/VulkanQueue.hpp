#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanDevice;
class VulkanCommandBuffer;
class VulkanSemaphore;
class VulkanFence;
class VulkanSwapchain;

class VulkanQueue
{
public:
    ~VulkanQueue();
    
    void Submit(
        std::shared_ptr<VulkanCommandBuffer> commandBuffer,
        VkPipelineStageFlags waitStageMask,
        std::shared_ptr<VulkanSemaphore> waitSemaphore,
        std::shared_ptr<VulkanSemaphore> signalSemaphore,
        std::shared_ptr<VulkanFence> fence
    );

    void Present(
        uint32_t imageIndex,
        std::shared_ptr<VulkanSwapchain> swapchain,
        std::shared_ptr<VulkanSemaphore> waitSemaphore = nullptr
    );

private:
    friend class VulkanDevice;
    VulkanQueue(VkQueue queue, std::shared_ptr<VulkanDevice> device, uint32_t familyIndex, uint32_t index);
    
private:
    VkQueue m_Queue;
    std::shared_ptr<VulkanDevice> m_Device;

    uint32_t m_FamilyIndex;
    uint32_t m_Index;
};