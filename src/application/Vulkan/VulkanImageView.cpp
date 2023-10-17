#include "VulkanImageView.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"

VulkanImageView::VulkanImageView()
    : m_ImageView(VK_NULL_HANDLE), m_Swapchain(VK_NULL_HANDLE), m_Device(nullptr)
{}

VulkanImageView::VulkanImageView(std::shared_ptr<VulkanSwapchain> swapchain, VkImage swapchainImage, std::shared_ptr<VulkanDevice> device)
    : m_ImageView(VK_NULL_HANDLE), m_Swapchain(swapchain), m_Device(device)
{
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapchainImage;
    
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = swapchain->GetSurfaceFormat().format;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VkResult createResult = vkCreateImageView(m_Device->GetHandle(), &createInfo, nullptr, &m_ImageView);

    if(createResult != VK_SUCCESS)
    {
        Log.Error("vkCreateImageView failed");
        throw std::runtime_error("Vulkan error");
    }

    Log.Info("VulkanImageView created");
}

VulkanImageView::~VulkanImageView()
{
    if(m_ImageView != VK_NULL_HANDLE)
    {
        Log.Info("VulkanImageView destroyed");
        vkDestroyImageView(m_Device->GetHandle(), m_ImageView, nullptr);
    }
}