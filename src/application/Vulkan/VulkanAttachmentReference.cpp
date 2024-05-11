#include "VulkanAttachmentReference.hpp"

std::shared_ptr<VulkanAttachmentReference> VulkanAttachmentReference::Create(uint32_t attachment, VkImageLayout layout)
{
    return std::make_shared<VulkanAttachmentReference>(attachment, layout);
}