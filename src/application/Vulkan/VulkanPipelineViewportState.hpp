#pragma once

#include "VulkanViewport.hpp"
#include "VulkanRect2D.hpp"

class VulkanPipelineViewportState : public VkPipelineViewportStateCreateInfo
{
public:
    VulkanPipelineViewportState(const VulkanViewport& viewport, const VulkanRect2D& scissor);

    operator const VkPipelineViewportStateCreateInfo*() const { return this; }
public:
    VulkanViewport m_Viewport;
    VulkanRect2D m_Scissor;
};