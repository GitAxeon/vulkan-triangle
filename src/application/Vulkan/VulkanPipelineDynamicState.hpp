#pragma once

#include <Vulkan/vulkan.h>

#include <vector>

class VulkanPipelineDynamicState : public VkPipelineDynamicStateCreateInfo
{
public:
    VulkanPipelineDynamicState(const std::vector<VkDynamicState>& states);
    VulkanPipelineDynamicState(const VulkanPipelineDynamicState& other);
    VulkanPipelineDynamicState& operator=(const VulkanPipelineDynamicState& other);
    
    operator const VkPipelineDynamicStateCreateInfo*() const { return this; }
private:
    std::vector<VkDynamicState> m_States;
};