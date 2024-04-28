#include "VulkanPipelineColorBlendAttachment.hpp"
VulkanPipelineColorBlendAttachment::VulkanPipelineColorBlendAttachment
(
    bool blendEnable,
    VkColorComponentFlags colorWriteMask,
    VkBlendFactor srcColorBlendFactor,
    VkBlendFactor dstColorBlendFactor,
    VkBlendOp colorBlendOp,
    VkBlendFactor srcAlphaBlendFactor,
    VkBlendFactor dstAlphaBlendFactor,
    VkBlendOp alphaBlendOp
) : VkPipelineColorBlendAttachmentState
    {
        blendEnable ? VK_TRUE : VK_FALSE,
        srcColorBlendFactor,
        dstColorBlendFactor,
        colorBlendOp,
        srcAlphaBlendFactor,
        dstAlphaBlendFactor,
        alphaBlendOp,
        colorWriteMask
    }
{
}