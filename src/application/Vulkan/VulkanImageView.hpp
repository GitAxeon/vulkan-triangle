#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanDevice;
class VulkanSwapchain;

class VulkanImageView
{
public:
    VulkanImageView();
    VulkanImageView(std::shared_ptr<VulkanSwapchain> swapchain, VkImage swapchainImage, std::shared_ptr<VulkanDevice> device);
    ~VulkanImageView();

private:
    VkImageView m_ImageView;

    std::shared_ptr<VulkanSwapchain> m_Swapchain;
    std::shared_ptr<VulkanDevice> m_Device;
};