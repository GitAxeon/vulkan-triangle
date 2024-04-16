#pragma once

#include "VulkanDevice.hpp"

class VulkanPipelineLayout
{
public:
    VulkanPipelineLayout(std::shared_ptr<VulkanDevice> device, const VkPipelineLayoutCreateInfo& createInfo);
    ~VulkanPipelineLayout();

    VkPipelineLayout GetHandle() const;
private:
    std::shared_ptr<VulkanDevice> m_Device;
    VkPipelineLayout m_PipelineLayout;
};