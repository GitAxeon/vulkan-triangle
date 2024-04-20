#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanSubpassDependency : VkSubpassDependency
{
public:
    constexpr VulkanSubpassDependency(
        uint32_t srcSubpass,
        uint32_t dstSubpass,
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask,
        VkAccessFlags srcAccessMask,
        VkAccessFlags dstAccessMask,
        VkDependencyFlags dependencyFlags
    )
        : VkSubpassDependency { srcSubpass, dstSubpass, srcStageMask, dstStageMask, srcAccessMask, dstAccessMask, dependencyFlags }
    {
    }

    operator const VkSubpassDependency*() const { return static_cast<const VkSubpassDependency*>(this); }
};