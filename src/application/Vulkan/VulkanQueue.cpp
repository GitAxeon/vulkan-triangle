#include "VulkanQueue.hpp"

#include "VulkanDevice.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanFence.hpp"
#include "VulkanSwapchain.hpp"

VulkanQueue::VulkanQueue(VkQueue queue, std::shared_ptr<VulkanDevice> device, uint32_t familyIndex, uint32_t index)
    : m_Queue(queue), m_Device(device), m_FamilyIndex(familyIndex), m_Index(index)
{   
    Log.Info("Queue created");
}

VulkanQueue::~VulkanQueue()
{
    Log.Info("Queue destructed");
}

void VulkanQueue::Submit
(
    const VulkanCommandBuffer& commandBuffer,
    VkPipelineStageFlags waitStageMask,
    VulkanSemaphore* waitSemaphore,
    VulkanSemaphore* signalSemaphore,
    VulkanFence* fence
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
    submitInfo.pCommandBuffers = &commandBuffer.GetHandleAddress();

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

void VulkanQueue::Present(uint32_t imageIndex, const VulkanSwapchain& swapchain, VulkanSemaphore* waitSemaphore)
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

    VkSwapchainKHR swapChains[] = {swapchain.GetHandle()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(m_Queue, &presentInfo);
}