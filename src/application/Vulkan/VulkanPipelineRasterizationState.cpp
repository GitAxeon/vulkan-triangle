#include "VulkanPipelineRasterizationState.hpp"

VulkanPipelineRasterizationState::VulkanPipelineRasterizationState(
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
) : VkPipelineRasterizationStateCreateInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        VK_NULL_HANDLE,
        flags,
        depthClampEnable ? VK_TRUE : VK_FALSE,
        rasterizerDiscardEnable ? VK_TRUE : VK_FALSE,
        polygonMode,
        cullMode,
        frontFace,
        depthBiasEnable,
        depthBiasConstantFactor,
        depthBiasClamp,
        depthBiasSlopeFactor,
        lineWidth
    }
{
}