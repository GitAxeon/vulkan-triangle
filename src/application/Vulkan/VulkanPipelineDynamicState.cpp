#include "VulkanPipelineDynamicState.hpp"

VulkanPipelineDynamicState::VulkanPipelineDynamicState
(
    const std::vector<VkDynamicState>& states
)
    : VkPipelineDynamicStateCreateInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        VK_NULL_HANDLE,
        0,
        static_cast<uint32_t>(states.size()),
        VK_NULL_HANDLE
    },
    m_States(states)
{
    pDynamicStates = m_States.data();
};

VulkanPipelineDynamicState::VulkanPipelineDynamicState
(
    const VulkanPipelineDynamicState& other
) : VkPipelineDynamicStateCreateInfo(other),
    m_States(other.m_States)
{
    pDynamicStates = m_States.data();
}

VulkanPipelineDynamicState& VulkanPipelineDynamicState::operator=(const VulkanPipelineDynamicState& other)
{
    if(this == &other)
        return *this;

    VkPipelineDynamicStateCreateInfo::operator=(other);

    m_States = other.m_States;

    pDynamicStates = m_States.data();

    return *this;    
}
