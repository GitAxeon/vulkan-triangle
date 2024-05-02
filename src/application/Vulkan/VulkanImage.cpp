#include "VulkanImage.hpp"

VulkanImage::VulkanImage
(
    std::shared_ptr<VulkanDevice> device,
    VkImage handle,
    VkExtent2D extent,
    VkFormat format
) : m_Image(handle), m_Device(device), m_Extent(extent), m_Format(format)
{
    Log.Info("Image create");
}

VulkanImage::~VulkanImage()
{
    vkDestroyImage(m_Device->GetHandle(), m_Image, nullptr);
    Log.Info("Image destructed");
}

std::shared_ptr<VulkanDevice> VulkanImage::GetDevice() const
{
    return m_Device;
}

VkImage VulkanImage::GetHandle() const
{
    return m_Image;
}

VkExtent2D VulkanImage::GetExtent() const
{
    return m_Extent;
}

VkFormat VulkanImage::GetFormat() const
{
    return m_Format;
}