#pragma once

#include "VulkanPipelineColorBlendAttachment.hpp"

#include <vector>
#include <array>

class VulkanPipelineColorBlendState : public VkPipelineColorBlendStateCreateInfo
{
public:
    VulkanPipelineColorBlendState
    (
        bool logicOpEnable,
        VkLogicOp logicOp,
        const std::vector<VulkanPipelineColorBlendAttachment>& attachments,
        const std::array<float, 4>& floats = {0, 0, 0, 0}
    );
};