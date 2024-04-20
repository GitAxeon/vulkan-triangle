#include "VulkanRenderPass.hpp"

VulkanRenderPass::VulkanRenderPass(std::shared_ptr<VulkanDevice> device)
    : m_Device(device), m_RenderPass(VK_NULL_HANDLE)
{
    
}

VulkanRenderPass::~VulkanRenderPass()
{
    vkDestroyRenderPass(m_Device->GetHandle(), m_RenderPass, nullptr);
}