#include "VulkanSwapchainImage.hpp"

VulkanSwapchainImage::VulkanSwapchainImage(std::shared_ptr<VulkanDevice> device, VkImage handle, VkFormat imageFormat, VkExtent2D extent, uint32_t index)
    : m_Device(device), m_Image(handle), m_Format(imageFormat), m_Extent(extent), m_Index(index)
{

}

VkImage VulkanSwapchainImage::GetHandle() const
{
    return m_Image;
}

VkFormat VulkanSwapchainImage::GetFormat() const
{
    return m_Format;
}

VkExtent2D VulkanSwapchainImage::GetExtent() const
{
    return m_Extent;
}

uint32_t VulkanSwapchainImage::GetIndex() const
{
    return m_Index;
}