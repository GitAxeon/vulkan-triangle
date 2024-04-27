#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanVertexInputBindingDescription : VkVertexInputBindingDescription
{
public:
    VulkanVertexInputBindingDescription(
        uint32_t binding, 
        uint32_t stride,
        VkVertexInputRate inputRate
    );

    operator const VkVertexInputBindingDescription*() const { return this; }
};