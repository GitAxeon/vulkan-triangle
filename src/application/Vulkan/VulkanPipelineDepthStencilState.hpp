#pragma once 

#include <Vulkan/vulkan.hpp>

class VulkanPipelineDepthStencilState : public VkPipelineDepthStencilStateCreateInfo
{
public:
    VulkanPipelineDepthStencilState(
        bool depthTestEnable,
        bool depthWriteEnable,
        VkCompareOp depthCompareOp,
        bool depthBoundsTestEnable,
        bool stencilTestEnable,
        VkStencilOpState front = {},
        VkStencilOpState back = {},
        float minDepthBounds = 0.0f,
        float maxDepthBounds = 1.0f
    );

    operator const VkPipelineDepthStencilStateCreateInfo*() const { return this; }
};