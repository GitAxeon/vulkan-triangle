#pragma once

#include "VulkanDevice.hpp"

class VulkanRenderPass
{
public:
    VulkanRenderPass(std::shared_ptr<VulkanDevice> device);
    ~VulkanRenderPass();

private:
    std::shared_ptr<VulkanDevice> m_Device;
    
    VkRenderPass m_RenderPass;
};