#pragma once

#include <Vulkan/vulkan.hpp>
#include <memory>

class VulkanDevice;
class VulkanSwapchain
{
public:
    VulkanSwapchain(std::shared_ptr<VulkanDevice> device)
        : m_Device(device)
    { } 
    
    std::shared_ptr<VulkanDevice> GetDevice() { return m_Device; }

private:
    std::shared_ptr<VulkanDevice> m_Device;
};  