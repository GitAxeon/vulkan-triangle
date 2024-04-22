#pragma once

#include "VulkanDevice.hpp"
#include "VulkanImageView.hpp"
#include "VulkanRenderPass.hpp"

class VulkanRenderPass;

class VulkanFramebuffer
{
public:
    VulkanFramebuffer(std::shared_ptr<VulkanRenderPass> renderPass, std::shared_ptr<VulkanImageView> attachment, VkFramebufferCreateFlags flags = 0);
    ~VulkanFramebuffer();

    VkFramebuffer GetHandle() const;

private:
    VkFramebuffer m_Framebuffer;
    std::shared_ptr<VulkanRenderPass> m_RenderPass;
    std::shared_ptr<VulkanImageView> m_Attachment;

    VkExtent2D m_Extent;
};