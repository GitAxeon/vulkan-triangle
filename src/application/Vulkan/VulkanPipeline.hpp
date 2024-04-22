#pragma once

#include "VulkanDevice.hpp"

class VulkanPipeline
{
public:
    VulkanPipeline();
    ~VulkanPipeline();

    VkPipeline GetHandle() const;
    
private:
    VkPipeline m_Pipeline;
};