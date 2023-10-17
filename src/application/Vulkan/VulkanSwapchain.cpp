#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"

#include <limits>

VulkanSwapchain::VulkanSwapchain(std::shared_ptr<VulkanDevice> device, VkSurfaceKHR surface, const VulkanSwapchainPreferences& preferences)
    : m_Device(device)
{
    SwapchainSupportDetails swapchainSupport = m_Device->GetSwapchainSupportDetails(surface);

    m_SurfaceFormat = SelectSurfaceFormat(surface, preferences.SurfaceFormat);
    m_PresentMode = SelectPresentMode(surface, preferences.PresentMode);
    m_Extent = SelectExtent(surface, preferences);
    m_ImageCount = FigureImageCount(surface, preferences);

    VkSwapchainCreateInfoKHR createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = m_ImageCount;
    createInfo.imageFormat = m_SurfaceFormat.format;
    createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
    createInfo.imageExtent = m_Extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = preferences.ImageUsage;
    
    std::vector<uint32_t> familyIndices(preferences.QueueFamilyIndices.begin(), preferences.QueueFamilyIndices.end());

    if(familyIndices.size() > 1)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = familyIndices.size();
        createInfo.pQueueFamilyIndices = familyIndices.data();
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = (preferences.PreTransform == 0) ? swapchainSupport.Capabilities.currentTransform : preferences.PreTransform;
    createInfo.compositeAlpha = preferences.CompositeAlpha;
    createInfo.presentMode = m_PresentMode;
    createInfo.clipped = preferences.ClipObscured;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(m_Device->GetHandle(), &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
    {
        Log.Error("Failed to create swap chain");
        throw std::runtime_error("Vulkan error");
    }

    Log.Info("Swapchain created successfully");

    m_ImageCount = QueryImageCount();
}

VulkanSwapchain::~VulkanSwapchain()
{
    vkDestroySwapchainKHR(m_Device->GetHandle(), m_Swapchain, nullptr);
}

std::vector<VkImage> VulkanSwapchain::GetSwapchainImages() const
{
    VkResult operationResult;
    uint32_t imageCount = 0;
    
    operationResult = vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Swapchain, &imageCount, nullptr);

    if(operationResult != VK_SUCCESS)
    {
        Log.Error("VkGetSwapchainImagesKHR #1 failed");
        throw std::runtime_error("Vulkan error");
    }

    if(imageCount == 0)
    {
        Log.Error("Zero swapchain images available");
        throw std::runtime_error("Vulkan error");
    }
    
    std::vector<VkImage> images(imageCount);
    operationResult = vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Swapchain, &imageCount, images.data());

    if(operationResult != VK_SUCCESS)
    {
        Log.Error("VkGetSwapchainImagesKHR #2 failed");
        throw std::runtime_error("Vulkan error");
    }
    
    return images;
}

uint32_t VulkanSwapchain::QueryImageCount() const
{
    VkResult operationResult;
    uint32_t imageCount = 0;
    
    operationResult = vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Swapchain, &imageCount, nullptr);

    if(operationResult != VK_SUCCESS)
    {
        Log.Error("VkGetSwapchainImagesKHR #1 failed");
        throw std::runtime_error("Vulkan error");
    }

    return imageCount;
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
        imageCount = capabilities.minImageCount + 1;

    if(capabilities.maxImageCount == 0)
        return imageCount;


    return std::clamp(imageCount, capabilities.minImageCount, capabilities.maxImageCount);
}