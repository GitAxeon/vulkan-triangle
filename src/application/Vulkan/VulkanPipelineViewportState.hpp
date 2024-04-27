#pragma once

#include "VulkanViewport.hpp"
#include "VulkanRect2D.hpp"

class VulkanPipelineViewportState : public VkPipelineViewportStateCreateInfo
{
public:
    VulkanPipelineViewportState(const VulkanViewport& viewport, const VulkanRect2D& scissor);

public:
    VulkanViewport m_Viewport;
    VulkanRect2D m_Scissor;
};