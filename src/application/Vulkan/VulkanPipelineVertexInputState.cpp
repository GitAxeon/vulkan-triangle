#include "VulkanPipelineVertexInputState.hpp"

#include "VulkanVertexInputBindingDescription.hpp"

VulkanPipelineVertexInputState::VulkanPipelineVertexInputState()
    : VkPipelineVertexInputStateCreateInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        VK_NULL_HANDLE,
        0,
        0,
        VK_NULL_HANDLE,
        0,
        VK_NULL_HANDLE
    }
{
}

// INCOMPLETE
VulkanPipelineVertexInputState::VulkanPipelineVertexInputState
(
    const std::vector<VulkanVertexInputBindingDescription>& bindings,
    const std::vector<VulkanVertexInputAttributeDescription>& attributes
) : VkPipelineVertexInputStateCreateInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        VK_NULL_HANDLE,
        0,
        static_cast<uint32_t>(bindings.size()),
        VK_NULL_HANDLE,
        0,
        VK_NULL_HANDLE
    }
{
}
