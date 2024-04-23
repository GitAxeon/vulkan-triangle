#pragma once

#include <Vulkan/vulkan.h>

#include <vector>

class VulkanPipelineDynamicState : public VkPipelineDynamicStateCreateInfo
{
public:
    VulkanPipelineDynamicState(const std::vector<VkDynamicState>& states);
    VulkanPipelineDynamicState(const VulkanPipelineDynamicState& other);
    VulkanPipelineDynamicState& operator=(const VulkanPipelineDynamicState& other);
    
private:
    std::vector<VkDynamicState> m_States;
};