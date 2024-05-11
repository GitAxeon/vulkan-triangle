#pragma once

#include "VulkanDevice.hpp"
#include "VulkanAttachmentDescription.hpp"
#include "VulkanSubpassDescription.hpp"
#include "VulkanSubpassDependency.hpp"

class VulkanRenderPass
{
public:
    VulkanRenderPass(
        std::shared_ptr<VulkanDevice> device,
        const VulkanAttachmentDescription& attachment,
        const VulkanSubpassDescription& subpass,
        const VulkanSubpassDependency& dependency
    );

    ~VulkanRenderPass();

    static std::shared_ptr<VulkanRenderPass> Create(
        std::shared_ptr<VulkanDevice> device,
        const VulkanAttachmentDescription& attachment,
        const VulkanSubpassDescription& subpass,
        const VulkanSubpassDependency& dependency
    );

    VkRenderPass GetHandle() const;
    std::shared_ptr<VulkanDevice> GetDevice() const;

private:
    std::shared_ptr<VulkanDevice> m_Device;
    
    VkRenderPass m_RenderPass;
};