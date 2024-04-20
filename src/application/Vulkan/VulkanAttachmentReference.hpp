#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanAttachmentReference : VkAttachmentReference
{
public:
    constexpr VulkanAttachmentReference(uint32_t attachment, VkImageLayout layout)
        : VkAttachmentReference { attachment, layout }
    { }

    operator const VkAttachmentReference*() const { return static_cast<const VkAttachmentReference*>(this); }
};
