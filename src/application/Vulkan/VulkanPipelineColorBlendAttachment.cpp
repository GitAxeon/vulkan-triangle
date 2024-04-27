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
        blendEnable,
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