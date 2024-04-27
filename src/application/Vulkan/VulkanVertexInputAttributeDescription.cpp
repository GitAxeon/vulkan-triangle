#include "VulkanVertexInputAttributeDescription.hpp"

VulkanVertexInputAttributeDescription::VulkanVertexInputAttributeDescription(
    uint32_t location,
    uint32_t binding,
    VkFormat format,
    uint32_t offset
) : VkVertexInputAttributeDescription
    {
        location,
        binding,
        format, 
        offset
    }
{
}