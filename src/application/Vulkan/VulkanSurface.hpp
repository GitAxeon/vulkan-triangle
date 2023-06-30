#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanPhysicalDevice;

class VulkanSurface
{
public:
    VulkanSurface(std::shared_ptr<VulkanPhysicalDevice> physicalDevice, VkSurfaceKHR surface)
        : m_PhysicalDevice(physicalDevice), m_Surface(surface)
    {
    }

private:
    std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
    VkSurfaceKHR m_Surface;
};
