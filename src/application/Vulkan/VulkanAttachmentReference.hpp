#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanAttachmentReference : VkAttachmentReference
{
public:
    constexpr VulkanAttachmentReference(uint32_t attachment, VkImageLayout layout)
        : VkAttachmentReference { attachment, layout }
    { }

    static std::shared_ptr<VulkanAttachmentReference> Create(uint32_t attachment, VkImageLayout layout);

    operator const VkAttachmentReference*() const { return this; }
};
