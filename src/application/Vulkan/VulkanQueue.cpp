#include "VulkanQueue.hpp"

#include "VulkanDevice.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanFence.hpp"
#include "VulkanSwapchain.hpp"

VulkanQueue::VulkanQueue(VkQueue queue, std::shared_ptr<VulkanDevice> device, uint32_t familyIndex, uint32_t index)
    : m_Queue(queue), m_Device(device), m_FamilyIndex(familyIndex), m_Index(index)
{
}

void VulkanQueue::Submit
(
    std::shared_ptr<VulkanCommandBuffer> commandBuffer,
    VkPipelineStageFlags waitStageMask = 0,
    std::shared_ptr<VulkanSemaphore> waitSemaphore = nullptr,
    std::shared_ptr<VulkanSemaphore> signalSemaphore = nullptr,
    std::shared_ptr<VulkanFence> fence = nullptr
)
{
    VkSemaphore waitSemaphores[] = { waitSemaphore->GetHandle() };
    VkPipelineStageFlags waitStages[] = {waitStageMask};

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer->GetHandleAddress();

    VkSemaphore signalSemaphores[] = {signalSemaphore->GetHandle()};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkResult submitResult = vkQueueSubmit(m_Queue, 1, &submitInfo, fence->GetHandle());

    if(submitResult != VK_SUCCESS)
    {
        Log.Error("Failed to submit draw command buffer");
        throw std::runtime_error("Vulkan error");
    }
}

void VulkanQueue::Present(uint32_t imageIndex, std::shared_ptr<VulkanSwapchain> swapchain, std::shared_ptr<VulkanSemaphore> waitSemaphore)
{
    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    if(waitSemaphore)
    {
        presentInfo.waitSemaphoreCount = 1;
        VkSemaphore semaphore = waitSemaphore->GetHandle();
        presentInfo.pWaitSemaphores = &semaphore;
    }
    else
    {
        presentInfo.waitSemaphoreCount = 0;
        presentInfo.pWaitSemaphores = nullptr;
    }

    VkSwapchainKHR swapChains[] = {swapchain->GetHandle()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(m_Queue, &presentInfo);
}