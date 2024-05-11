#pragma once

#include <memory>

#include "VulkanDevice.hpp"

class VulkanSwapchain;
class VulkanImage;


class VulkanImageView
{
public:
    constexpr static VkComponentMapping DefaultComponentMapping = { 
        VK_COMPONENT_SWIZZLE_IDENTITY,
        VK_COMPONENT_SWIZZLE_IDENTITY,
        VK_COMPONENT_SWIZZLE_IDENTITY,
        VK_COMPONENT_SWIZZLE_IDENTITY
    };
    
    VulkanImageView(
        std::shared_ptr<VulkanImage> image,
        VkComponentMapping mapping = DefaultComponentMapping
    );

    ~VulkanImageView();

    static std::shared_ptr<VulkanImageView> Create(
        std::shared_ptr<VulkanImage> image,
        VkComponentMapping mapping = DefaultComponentMapping
    );

    VkImageView GetHandle() const;
    std::shared_ptr<VulkanImage> GetImage() const;

    VkExtent2D GetExtent() const;

private:
    VkImageView m_ImageView;
    std::shared_ptr<VulkanImage> m_Image;
};