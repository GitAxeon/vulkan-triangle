#include "VulkanVertexInputBindingDescription.hpp"

VulkanVertexInputBindingDescription::VulkanVertexInputBindingDescription(
    uint32_t binding, 
    uint32_t stride,
    VkVertexInputRate inputRate
) : VkVertexInputBindingDescription
    {
        binding,
        stride,
        inputRate
    }
{
}