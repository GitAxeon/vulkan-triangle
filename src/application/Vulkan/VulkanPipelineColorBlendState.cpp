#include "VulkanPipelineColorBlendState.hpp"

VulkanPipelineColorBlendState::VulkanPipelineColorBlendState
(
    bool logicOpEnable,
    VkLogicOp logicOp,
    const std::vector<VulkanPipelineColorBlendAttachment>& attachments,
    const std::array<float, 4>& blendConstants
) : VkPipelineColorBlendStateCreateInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        VK_NULL_HANDLE,
        0,
        logicOpEnable ? VK_TRUE : VK_FALSE,
        logicOp,
        static_cast<uint32_t>(attachments.size()),
        attachments.data(),
        { blendConstants[0], blendConstants[1], blendConstants[2], blendConstants[3] }
    }
{

}