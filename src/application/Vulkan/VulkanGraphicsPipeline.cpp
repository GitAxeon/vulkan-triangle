#include "VulkanGraphicsPipeline.hpp"

#include "VulkanDevice.hpp"
#include "VulkanPipelineShaderStage.hpp"
#include "VulkanPipelineVertexInputState.hpp"
#include "VulkanPipelineInputAssemblyState.hpp"
#include "VulkanPipelineViewportState.hpp"
#include "VulkanPipelineRasterizationState.hpp"
#include "VulkanPipelineMultisampleState.hpp"
#include "VulkanPipelineDepthStencilState.hpp"
#include "VulkanPipelineColorBlendState.hpp"
#include "VulkanPipelineDynamicState.hpp"
#include "VulkanPipelineLayout.hpp"
#include "VulkanRenderPass.hpp"

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
    int32_t subpass,
    std::shared_ptr<VulkanGraphicsPipeline> basePipeline,
    int32_t basePipelineIndex
)
    : VulkanPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, device, layout)
{
    VkGraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = vertexInputState;
    pipelineInfo.pInputAssemblyState = inputAssemblyState;
    pipelineInfo.pViewportState = viewportState;
    pipelineInfo.pRasterizationState = rasterizationState;
    pipelineInfo.pMultisampleState = multisampleState;
    pipelineInfo.pDepthStencilState = depthStensiclState;
    pipelineInfo.pColorBlendState = colorBlendState;
    pipelineInfo.pDynamicState = dynamicState;
    pipelineInfo.layout = layout->GetHandle();
    pipelineInfo.renderPass = renderPass->GetHandle();
    pipelineInfo.subpass = subpass;
    pipelineInfo.basePipelineHandle = basePipeline ? basePipeline->GetHandle() : VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = basePipelineIndex;

    VkResult result = vkCreateGraphicsPipelines(device->GetHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline);

    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to create graphics pipeline");
        throw std::runtime_error("Vulkan error");
    }
}