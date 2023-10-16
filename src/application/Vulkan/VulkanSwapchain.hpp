#pragma once

#include <Vulkan/vulkan.hpp>

#include <functional>
#include <memory>

/*
    User filled object representing swapchain preferences
*/
class VulkanSwapchainPreferences
{
public:
    VulkanSwapchainPreferences()
        :   SurfaceFormat({VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}),
            PresentMode(VK_PRESENT_MODE_FIFO_KHR),
            ImageCount(0),
            WindowSizeInPixels({0, 0})
    { }

public:
    VkSurfaceFormatKHR SurfaceFormat;
    VkPresentModeKHR PresentMode;
    uint32_t ImageCount;
    VkExtent2D WindowSizeInPixels;
};

class VulkanDevice;
class VulkanSwapchainPreferences;

class VulkanSwapchain
{
public:
    VulkanSwapchain(std::shared_ptr<VulkanDevice> device, VkSurfaceKHR surface, const VulkanSwapchainPreferences& preferences);
    ~VulkanSwapchain();
    
    std::shared_ptr<VulkanDevice> GetDevice() { return m_Device; }

private:
    VkSurfaceFormatKHR SelectSurfaceFormat(const VkSurfaceKHR surface, const VkSurfaceFormatKHR& preference);
    VkPresentModeKHR SelectPresentMode(const VkSurfaceKHR surface, const VkPresentModeKHR preference);
    VkExtent2D SelectExtent(const VkSurfaceKHR surface, const VulkanSwapchainPreferences& preferences);
    uint32_t FigureImageCount(const VkSurfaceKHR surface, const VulkanSwapchainPreferences& preferences);

private:
    VkSurfaceFormatKHR m_SurfaceFormat;
    VkExtent2D Extent;

    std::shared_ptr<VulkanDevice> m_Device;
};  