#pragma once

#include "VulkanDevice.hpp"

class VulkanImage
{
public:
    VulkanImage();
    ~VulkanImage();

    VkImage GetHandle() const;

private:
    std::shared_ptr<VulkanDevice> m_Device;
    VkImage m_Image;
};