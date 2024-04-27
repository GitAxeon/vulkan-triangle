#include "VulkanPipelineInputAssemblyState.hpp"

VulkanPipelineInputAssemblyState::VulkanPipelineInputAssemblyState
(
    VkPrimitiveTopology topology, 
    bool enablePrimitiveRestart
) : VkPipelineInputAssemblyStateCreateInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        VK_NULL_HANDLE,
        0,
        topology,
        enablePrimitiveRestart ? VK_TRUE : VK_FALSE
    }
{
}