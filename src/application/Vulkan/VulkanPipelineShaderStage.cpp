#include "VulkanPipelineShaderStage.hpp"

#include "VulkanShaderModule.hpp"

VulkanPipelineShaderStage::VulkanPipelineShaderStage
(
    VkShaderStageFlagBits shaderStageFlag,
    std::shared_ptr<VulkanShaderModule> module,
    const char* entryPoint = "main",
    VkPipelineShaderStageCreateFlags createFlags = 0
) 
    : VkPipelineShaderStageCreateInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        VK_NULL_HANDLE,
        0,
        shaderStageFlag,
        module->GetHandle(),
        entryPoint,
        VK_NULL_HANDLE
    }
{}