#include "VulkanPipelineDepthStencilState.hpp"

VulkanPipelineDepthStencilState::VulkanPipelineDepthStencilState
(
    bool depthTestEnable,
    bool depthWriteEnable,
    VkCompareOp depthCompareOp,
    bool depthBoundsTestEnable,
    bool stencilTestEnable,
    VkStencilOpState front,
    VkStencilOpState back,
    float minDepthBounds,
    float maxDepthBounds
) : VkPipelineDepthStencilStateCreateInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        VK_NULL_HANDLE,
        0,
        depthTestEnable,
        depthWriteEnable,
        depthCompareOp,
        depthBoundsTestEnable,
        stencilTestEnable,
        front,
        back,
        minDepthBounds,
        maxDepthBounds
    }
{
}