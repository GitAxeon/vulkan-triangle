#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanRect2D : public VkRect2D
{
public:
    constexpr VulkanRect2D()
        : VkRect2D { {0, 0}, {0, 0}}
    {}
    
    constexpr VulkanRect2D(int32_t x, int32_t y, uint32_t width, uint32_t height)
        : VkRect2D { {x, y}, {width, height} }
    {}

    constexpr VulkanRect2D(VkExtent2D extent)
        : VkRect2D { {0, 0}, extent}
    {}

    operator const VkRect2D*() const { return this; }
};