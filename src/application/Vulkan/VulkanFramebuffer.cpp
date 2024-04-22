#include "VulkanFramebuffer.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanImage.hpp"

VulkanFramebuffer::VulkanFramebuffer(std::shared_ptr<VulkanRenderPass> renderPass, std::shared_ptr<VulkanImageView> attachment, VkFramebufferCreateFlags flags)
    : m_Framebuffer(VK_NULL_HANDLE), m_RenderPass(renderPass), m_Attachment(attachment)
{
    VkImageView attachments[] = {
        attachment->GetHandle()
    };

    const VkExtent2D extent = attachment->GetExtent();
    m_Extent = extent;

    VkFramebufferCreateInfo framebufferInfo {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass->GetHandle();
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width =  extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    VkResult createFramebufferResult = vkCreateFramebuffer(renderPass->GetDevice()->GetHandle(), &framebufferInfo, nullptr, &m_Framebuffer);
    
    if(createFramebufferResult != VK_SUCCESS)
    {
        Log.Error("Failed to create framebuffer");
        throw std::runtime_error("Vulkan error");
    }
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    vkDestroyFramebuffer(m_RenderPass->GetDevice()->GetHandle(), m_Framebuffer, nullptr);
}

VkFramebuffer VulkanFramebuffer::GetHandle() const
{
    return m_Framebuffer;
}

