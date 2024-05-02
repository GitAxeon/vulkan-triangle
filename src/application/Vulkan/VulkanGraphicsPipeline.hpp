#pragma once

#include "VulkanPipeline.hpp"

class VulkanPipelineShaderStage;
class VulkanPipelineVertexInputState;
class VulkanPipelineInputAssemblyState;
class VulkanPipelineViewportState;
class VulkanPipelineRasterizationState;
class VulkanPipelineMultisampleState;
class VulkanPipelineDepthStencilState;
class VulkanPipelineColorBlendState;
class VulkanPipelineDynamicState;
class VulkanPipelineLayout;
class VulkanRenderPass;

class VulkanGraphicsPipeline : public VulkanPipeline
{
public:
    VulkanGraphicsPipeline(
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
        int32_t subpass,
        std::shared_ptr<VulkanGraphicsPipeline> basePipeline = nullptr,
        int32_t basePipelineIndex = -1
    );
    
    ~VulkanGraphicsPipeline();
};