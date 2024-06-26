#include "VulkanRenderPass.hpp"

VulkanRenderPass::VulkanRenderPass
(
    std::shared_ptr<VulkanDevice> device,
    const VulkanAttachmentDescription& attachment,
    const VulkanSubpassDescription& subpass,
    const VulkanSubpassDependency& dependency
)
    : m_Device(device), m_RenderPass(VK_NULL_HANDLE)
{
    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = attachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = dependency;

    VkResult createRenderPassResult = vkCreateRenderPass(device->GetHandle(), &renderPassInfo, nullptr, &m_RenderPass);

    if(createRenderPassResult != VK_SUCCESS)
    {
        Log.Error("Failed to create render pass");
        throw std::runtime_error("Vulkan error");
    }

    Log.Info("RenderPass created");
}

VulkanRenderPass::~VulkanRenderPass()
{
    vkDestroyRenderPass(m_Device->GetHandle(), m_RenderPass, nullptr);
    Log.Info("RenderPass destructed");
}

std::shared_ptr<VulkanRenderPass> VulkanRenderPass::Create(
    std::shared_ptr<VulkanDevice> device,
    const VulkanAttachmentDescription& attachment,
    const VulkanSubpassDescription& subpass,
    const VulkanSubpassDependency& dependency
)
{
    return std::make_shared<VulkanRenderPass>(device, attachment, subpass, dependency);
}

VkRenderPass VulkanRenderPass::GetHandle() const
{
    return m_RenderPass;
}

std::shared_ptr<VulkanDevice> VulkanRenderPass::GetDevice() const
{
    return m_Device;
}