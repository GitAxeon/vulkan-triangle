#include "VulkanSemaphore.hpp"

VulkanSemaphore::VulkanSemaphore(std::shared_ptr<VulkanDevice> device)
    : m_Device(device), m_Semaphore(VK_NULL_HANDLE)
{
    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult result = vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &m_Semaphore);

    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to create semaphore");
        throw std::runtime_error("Vulkan error");
    }

    Log.Info("Semaphore created");
}

VulkanSemaphore::~VulkanSemaphore()
{
    vkDestroySemaphore(m_Device->GetHandle(), m_Semaphore, nullptr);
    Log.Info("Semaphore destructed");
}

VkSemaphore VulkanSemaphore::GetHandle() const
{
    return m_Semaphore;
}