#pragma once

#include "VulkanAttachmentReference.hpp"

#include <Vulkan/vulkan.hpp>

class VulkanSubpassDescription : VkSubpassDescription
{
public:
    VulkanSubpassDescription()
        : VkSubpassDescription
        {
            0,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            0,
            VK_NULL_HANDLE,
            0,
            VK_NULL_HANDLE,
            VK_NULL_HANDLE,
            VK_NULL_HANDLE,
            0,
            VK_NULL_HANDLE
        }
    {}

    VulkanSubpassDescription(VkPipelineBindPoint pipelineBindPoint)
        : VkSubpassDescription
        {
            0, 
            pipelineBindPoint,
            0,
            VK_NULL_HANDLE,
            0,
            VK_NULL_HANDLE,
            VK_NULL_HANDLE,
            VK_NULL_HANDLE,
            0,
            VK_NULL_HANDLE
        }
    {}

    VulkanSubpassDescription(std::shared_ptr<VulkanAttachmentReference> colorAttachment)
        : VulkanSubpassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS)
    {
        this->colorAttachmentCount = 1;
        this->pColorAttachments = *colorAttachment;
    }

    operator const VkSubpassDescription*() const { return this; }
};