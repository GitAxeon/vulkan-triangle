#pragma once

#include <Vulkan/vulkan.hpp>
class VulkanViewport : VkViewport
{
public:
    constexpr VulkanViewport()
        : VkViewport { 0, 0, 0, 0, 0, 0 }
    {}

    constexpr VulkanViewport( 
        float    x,
        float    y,
        float    width,
        float    height,
        float    minDepth,
        float    maxDepth
    )
        : VkViewport {x, y, width, height, minDepth, maxDepth }
    { }

    operator const VkViewport*() const { return static_cast<const VkViewport*>(this); }
}; 