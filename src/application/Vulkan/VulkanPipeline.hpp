#pragma once

#include "VulkanDevice.hpp"

class VulkanPipelineLayout;

class VulkanPipeline
{
public:
    ~VulkanPipeline();

    VkPipeline GetHandle() const;
    std::shared_ptr<VulkanDevice> GetDevice() const;

protected:
    VulkanPipeline(
        VkPipelineBindPoint bindPoint,
        std::shared_ptr<VulkanDevice> device,
        std::shared_ptr<VulkanPipelineLayout> layout
    );
    
protected:
    VkPipeline m_Pipeline;
    std::shared_ptr<VulkanDevice> m_Device;

    VkPipelineBindPoint m_BindPoint;
    std::shared_ptr<VulkanPipelineLayout> m_Layout;
};