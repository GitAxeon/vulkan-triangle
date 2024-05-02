#include "VulkanImageView.hpp"

#include "VulkanSwapchain.hpp"
#include "VulkanImage.hpp"

VulkanImageView::VulkanImageView(std::shared_ptr<VulkanImage> image, VkComponentMapping mapping)
    : m_ImageView(VK_NULL_HANDLE), m_Image(image)
{
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image->GetHandle();
    
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = image->GetFormat();

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VkResult createResult = vkCreateImageView(m_Image->GetDevice()->GetHandle(), &createInfo, nullptr, &m_ImageView);

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
        vkDestroyImageView(m_Image->GetDevice()->GetHandle(), m_ImageView, nullptr);
    }
    
    Log.Info("VulkanImageView destructed");
}

VkImageView VulkanImageView::GetHandle() const
{
    return m_ImageView;
}

VkExtent2D VulkanImageView::GetExtent() const
{
    return m_Image->GetExtent();
}