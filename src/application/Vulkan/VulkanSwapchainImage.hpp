#pragma once

#include "VulkanImage2D.hpp"

// Vulkan internally handles these images so they don't need to be destroyed using vkDestroyImage
class VulkanSwapchainImage : public VulkanImage2D
{
public:
    VulkanSwapchainImage(std::shared_ptr<VulkanDevice> device, VkImage handle, VkFormat imageFormat, VkExtent2D extent, uint32_t index);
    ~VulkanSwapchainImage();
    
    uint32_t GetIndex() const;

private:
    uint32_t m_Index;
};