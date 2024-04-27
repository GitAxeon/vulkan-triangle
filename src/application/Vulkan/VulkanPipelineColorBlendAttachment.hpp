#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanPipelineColorBlendAttachment : public VkPipelineColorBlendAttachmentState
{
public:
    VulkanPipelineColorBlendAttachment
    (
        bool blendEnable,
        VkColorComponentFlags colorWriteMask,
        VkBlendFactor srcColorBlendFactor,
        VkBlendFactor dstColorBlendFactor,
        VkBlendOp colorBlendOp,
        VkBlendFactor srcAlphaBlendFactor,
        VkBlendFactor dstAlphaBlendFactor,
        VkBlendOp alphaBlendOp
    );

    constexpr operator const VkPipelineColorBlendAttachmentState*() const { return this; }
};