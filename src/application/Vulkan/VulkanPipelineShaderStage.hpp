#pragma once

#include <Vulkan/vulkan.hpp>
class VulkanShaderModule;

class VulkanPipelineShaderStage : public VkPipelineShaderStageCreateInfo
{
public:
    VulkanPipelineShaderStage(
        VkShaderStageFlagBits shaderStageFlag,
        std::shared_ptr<VulkanShaderModule> module,
        const char* entryPoint = "main",
        VkPipelineShaderStageCreateFlags createFlags = 0
    );

    operator const VkPipelineShaderStageCreateInfo*() const { return this; }
};