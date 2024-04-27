#include "VulkanPipelineViewportState.hpp"
VulkanPipelineViewportState::VulkanPipelineViewportState
(
    const VulkanViewport& viewport,
    const VulkanRect2D& scissor
) : VkPipelineViewportStateCreateInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        VK_NULL_HANDLE,
        0,
        1,
        m_Viewport,
        1,
        m_Scissor
    }, m_Viewport(viewport), m_Scissor(scissor)
{
}
    