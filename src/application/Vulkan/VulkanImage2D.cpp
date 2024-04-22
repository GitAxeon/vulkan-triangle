#include "VulkanImage2D.hpp"

VulkanImage2D::VulkanImage2D(std::shared_ptr<VulkanDevice> device, VkImage handle, VkFormat format, VkExtent2D extent)
    : VulkanImage(device, handle, extent, format)
{
}