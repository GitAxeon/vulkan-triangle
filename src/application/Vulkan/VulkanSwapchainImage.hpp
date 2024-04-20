#pragma once

#include "VulkanDevice.hpp"

// Vulkan internally handles these images so they don't need to be destroyed using vkDestroyImage
class VulkanSwapchainImage
{
public:
    VulkanSwapchainImage(std::shared_ptr<VulkanDevice> device, VkImage handle, VkFormat imageFormat, VkExtent2D extent, uint32_t index);

    VkImage GetHandle() const;
    VkFormat GetFormat() const;
    VkExtent2D GetExtent() const;
    uint32_t GetIndex() const;

private:
    std::shared_ptr<VulkanDevice> m_Device;
    VkImage m_Image;
    VkFormat m_Format;
    VkExtent2D m_Extent;
    uint32_t m_Index;
};