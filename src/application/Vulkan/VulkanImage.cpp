#include "VulkanImage.hpp"

VulkanImage::VulkanImage()
{
    
}

VulkanImage::~VulkanImage()
{
    vkDestroyImage(m_Device->GetHandle(), m_Image, nullptr);
}

VkImage VulkanImage::GetHandle() const
{
    return m_Image;
}