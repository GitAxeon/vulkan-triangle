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
        const VulkanCommandBuffer& commandBuffer,
        VkPipelineStageFlags waitStageMask = 0,
        VulkanSemaphore* waitSemaphore = nullptr,
        VulkanSemaphore* signalSemaphore = nullptr,
        VulkanFence* fence = nullptr
    );

    VkResult Present(
        uint32_t imageIndex,
        const VulkanSwapchain& swapchain,
        VulkanSemaphore* waitSemaphore = nullptr
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