#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanPipelineInputAssemblyState : public VkPipelineInputAssemblyStateCreateInfo
{
public:
    VulkanPipelineInputAssemblyState(VkPrimitiveTopology topology, bool enablePrimitiveRestart);

    operator const VkPipelineInputAssemblyStateCreateInfo*() const { return this; }
};