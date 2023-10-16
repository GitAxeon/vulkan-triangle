#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"

#include <limits>

VulkanSwapchain::VulkanSwapchain(std::shared_ptr<VulkanDevice> device, VkSurfaceKHR surface, const VulkanSwapchainPreferences& preferences)
    : m_Device(device)
{
    VkSurfaceFormatKHR surfaceFormat = SelectSurfaceFormat(surface, preferences.SurfaceFormat);
    VkPresentModeKHR presentMode = SelectPresentMode(surface, preferences.PresentMode);
    VkExtent2D extent = SelectExtent(surface, preferences);
    uint32_t imageCount = FigureImageCount(surface, preferences);

    VkSwapchainCreateInfoKHR createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
}

VulkanSwapchain::~VulkanSwapchain()
{

}

VkSurfaceFormatKHR VulkanSwapchain::SelectSurfaceFormat(const VkSurfaceKHR surface, const VkSurfaceFormatKHR& preference)
{
    VkSurfaceFormatKHR result;

    auto formats = m_Device->GetSwapchainSupportDetails(surface).Formats;

    if(formats.empty())
    {
        Log.Error("No VkSurfaceFormatKHR available");
        throw std::runtime_error("Vulkan error");
    }

    for(auto value : formats)
    {
        if(value.format == preference.format && value.colorSpace == preference.colorSpace)
        {
            Log.Info("Preferred VkSurfaceFormatKHR selected");
            return value;
        }
    }
    
    Log.Info("Using first available VkSurfaceFormatKHR");
    return formats[0];
}

VkPresentModeKHR VulkanSwapchain::SelectPresentMode(const VkSurfaceKHR surface, const VkPresentModeKHR preference)
{
    VkPresentModeKHR result;

    auto presentModes = m_Device->GetSwapchainSupportDetails(surface).PresentModes;

    if(presentModes.empty())
    {
        Log.Error("No VkPresentModeKHR available");
        throw std::runtime_error("Vulkan error");
    }

    for(auto value : presentModes)
    {
        if(value == preference)
        {
            Log.Info("Preferred VkPresentModeKHR selected");
            return value;
        }
    }

    Log.Info("Using VK_PRESENT_MODE_FIFO_KHR as VkPresentModeKHR");
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::SelectExtent(const VkSurfaceKHR surface, const VulkanSwapchainPreferences& preferences)
{
    VkSurfaceCapabilitiesKHR capabilities = m_Device->GetSwapchainSupportDetails(surface).Capabilities;

    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    VkExtent2D extent = preferences.WindowSizeInPixels;

    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return extent;
}

uint32_t VulkanSwapchain::FigureImageCount(const VkSurfaceKHR surface, const VulkanSwapchainPreferences& preferences)
{
    VkSurfaceCapabilitiesKHR capabilities = m_Device->GetSwapchainSupportDetails(surface).Capabilities;

    uint32_t imageCount = preferences.ImageCount;

    if(imageCount == 0)
        imageCount == capabilities.minImageCount + 1;

    if(capabilities.maxImageCount == 0)
        return imageCount;


    return std::clamp(imageCount, capabilities.minImageCount, capabilities.maxImageCount);
}