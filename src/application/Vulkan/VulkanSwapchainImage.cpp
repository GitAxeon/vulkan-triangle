#include "VulkanSwapchainImage.hpp"

VulkanSwapchainImage::VulkanSwapchainImage(std::shared_ptr<VulkanDevice> device, VkImage handle, VkFormat imageFormat, VkExtent2D extent, uint32_t index)
    : VulkanImage2D(device, handle, imageFormat, extent), m_Index(index)
{ }

VulkanSwapchainImage::~VulkanSwapchainImage()
{
    // Prevent base class VulkanImage from destroying the image
    m_Image = VK_NULL_HANDLE;
}

uint32_t VulkanSwapchainImage::GetIndex() const
{
    return m_Index;
}