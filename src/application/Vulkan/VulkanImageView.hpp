#pragma once

#include <memory>

#include "VulkanDevice.hpp"

class VulkanSwapchain;
class VulkanImage;

class VulkanImageView
{
public:
    VulkanImageView(
        std::shared_ptr<VulkanImage> image,
        VkComponentMapping mapping = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }
    );

    ~VulkanImageView();

    VkImageView GetHandle() const;
    std::shared_ptr<VulkanImage> GetImage() const;

    VkExtent2D GetExtent() const;

private:
    VkImageView m_ImageView;
    std::shared_ptr<VulkanImage> m_Image;
};