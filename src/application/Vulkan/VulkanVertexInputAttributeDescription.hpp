#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanVertexInputAttributeDescription : public VkVertexInputAttributeDescription
{
public:
    VulkanVertexInputAttributeDescription(
        uint32_t location,
        uint32_t binding,
        VkFormat format,
        uint32_t offset
    );
    
    operator const VkVertexInputAttributeDescription*() const { return this; }
};