#pragma once

#include "VulkanPipeline.hpp"

class VulkanGraphicsPipeline : public VulkanPipeline
{
public:
    VulkanGraphicsPipeline(
        std::shared_ptr<VulkanDevice> device,
        std::vector<VkPipelineShaderStageCreateInfo>shaderStages
    );
};