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
        VkStencilOpState front,
        VkStencilOpState back,
        float minDepthBounds,
        float maxDepthBounds
    );
};