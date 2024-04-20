#include "VulkanFence.hpp"

VulkanFence::VulkanFence(std::shared_ptr<VulkanDevice> device, VkFenceCreateFlags flags)
    : m_Device(device), m_Fence(VK_NULL_HANDLE)
{
    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = flags;

    const VkResult result = vkCreateFence(device->GetHandle(), &fenceInfo, nullptr, &m_Fence);
    
    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to create fence");
        throw std::runtime_error("Vulkan error");
    }
}

VulkanFence::~VulkanFence()
{
    vkDestroyFence(m_Device->GetHandle(), m_Fence, nullptr);
}

VkResult VulkanFence::Wait(uint64_t timeout)
{
    const VkResult result = vkWaitForFences(m_Device->GetHandle(), 1, &m_Fence, VK_TRUE, timeout);
    
    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to wait for fence");
        throw std::runtime_error("Vulkan error");
    }

    return result;
}

VkResult VulkanFence::Reset()
{
    const VkResult result = vkResetFences(m_Device->GetHandle(), 1, &m_Fence);
    return result;
}

VkFence VulkanFence::GetHandle() const
{
    return m_Fence;
}