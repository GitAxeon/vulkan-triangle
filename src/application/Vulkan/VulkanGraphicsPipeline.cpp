#include "VulkanGraphicsPipeline.hpp"

VulkanGraphicsPipeline::VulkanGraphicsPipeline
(
    std::shared_ptr<VulkanDevice> device,
    const std::vector<VulkanPipelineShaderStage>& shaderStages,
    const VulkanPipelineVertexInputState& vertexInputState,
    const VulkanPipelineInputAssemblyState& inputAssemblyState,
    const VulkanPipelineViewportState& viewportState,
    const VulkanPipelineRasterizationState& rasterizationState,
    const VulkanPipelineMultisampleState& multisampleState,
    const VulkanPipelineDepthStencilState& depthStensiclState,
    const VulkanPipelineColorBlendState& colorBlendState,
    const VulkanPipelineDynamicState& dynamicState,
    std::shared_ptr<VulkanPipelineLayout> layout,
    std::shared_ptr<VulkanRenderPass> renderPass,
    uint32_t subpass,
    std::shared_ptr<VulkanGraphicsPipeline> basePipeline = nullptr,
    uint32_t basePipelineIndex = -1
)
{
    VkGraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
}