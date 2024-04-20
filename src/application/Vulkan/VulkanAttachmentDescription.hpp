#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanAttachmentDescription : VkAttachmentDescription
{
public:
    constexpr VulkanAttachmentDescription
    (
        VkFormat format,
        VkSampleCountFlagBits sampleCount,
        VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp,
        VkAttachmentLoadOp stencilLoadOp,
        VkAttachmentStoreOp stencilStoreOp,
        VkImageLayout initialLayout,
        VkImageLayout finalLayout
    )
        : VkAttachmentDescription { 0, format, sampleCount, loadOp, storeOp, stencilLoadOp, stencilStoreOp, initialLayout, finalLayout }
    {}

    constexpr operator const VkAttachmentDescription*() const { return static_cast<const VkAttachmentDescription*>(this); }
};
