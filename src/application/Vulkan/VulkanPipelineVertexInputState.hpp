#pragma once

#include "VulkanVertexInputBindingDescription.hpp"
#include "VulkanVertexInputAttributeDescription.hpp"

#include <Vulkan/vulkan.hpp>

#include <vector>

class VulkanPipelineVertexInputState : public VkPipelineVertexInputStateCreateInfo
{
public:
    VulkanPipelineVertexInputState();

    VulkanPipelineVertexInputState(
        const std::vector<VulkanVertexInputBindingDescription>& bindings,
        const std::vector<VulkanVertexInputAttributeDescription>& attributes
    );

    constexpr operator const VkPipelineVertexInputStateCreateInfo*() const { return this; }
};