#pragma once 

#include "VulkanImage.hpp"

class VulkanImage2D : public VulkanImage
{
public:
    VulkanImage2D(std::shared_ptr<VulkanDevice> device, VkImage handle, VkFormat format, VkExtent2D extent);
};