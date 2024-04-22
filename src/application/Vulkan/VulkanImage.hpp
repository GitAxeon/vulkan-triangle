#pragma once

#include "VulkanDevice.hpp"

class VulkanImage
{
public:
    ~VulkanImage();

    VkImage GetHandle() const;

    std::shared_ptr<VulkanDevice> GetDevice() const;
    VkExtent2D GetExtent() const;
    VkFormat GetFormat() const;

protected:
    VulkanImage(std::shared_ptr<VulkanDevice> device, VkImage handle, VkExtent2D extent, VkFormat format);

protected:
    VkImage m_Image;

    std::shared_ptr<VulkanDevice> m_Device;

    VkExtent2D m_Extent;
    VkFormat m_Format;
};