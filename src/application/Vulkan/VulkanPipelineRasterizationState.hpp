#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanPipelineRasterizationState : public VkPipelineRasterizationStateCreateInfo
{
public:
    VulkanPipelineRasterizationState(
        VkPolygonMode polygonMode,
        VkCullModeFlags cullMode,
        bool depthClampEnable,
        bool rasterizerDiscardEnable,
        VkFrontFace frontFace,
        bool depthBiasEnable,
        float lineWidth,
        float depthBiasConstantFactor = 0.0f,
        float depthBiasClamp = 0.0f,
        float depthBiasSlopeFactor = 0.0f,
        VkPipelineRasterizationStateCreateFlags flags = 0
    );
};