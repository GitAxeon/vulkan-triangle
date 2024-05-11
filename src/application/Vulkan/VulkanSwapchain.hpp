#pragma once

#include "VulkanSwapchainImage.hpp"

#include <Vulkan/vulkan.hpp>

#include <functional>
#include <memory>

/*
    User filled object representing swapchain preferences
*/
class VulkanSwapchainPreferences
{
public:
    VulkanSwapchainPreferences()
        :   SurfaceFormat({VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}),
            PresentMode(VK_PRESENT_MODE_FIFO_KHR),
            ImageCount(0),
            WindowSizeInPixels({0, 0}),
            ImageUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
            SharingMode(VK_SHARING_MODE_MAX_ENUM),
            QueueFamilyIndices({}),
            PreTransform(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR),
            CompositeAlpha(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR),
            ClipObscured(true)
    { }

public:
    VkSurfaceFormatKHR SurfaceFormat;
    VkPresentModeKHR PresentMode;
    uint32_t ImageCount;
    VkExtent2D WindowSizeInPixels;
    VkImageUsageFlags ImageUsage;
    
    VkSharingMode SharingMode;
    std::set<uint32_t> QueueFamilyIndices;

    VkSurfaceTransformFlagBitsKHR PreTransform;
    VkCompositeAlphaFlagBitsKHR CompositeAlpha;
    bool ClipObscured;
};

class VulkanDevice;
class VulkanSwapchainPreferences;
class VulkanSemaphore;

class VulkanSwapchain
{
public:
    struct AcquisitionResult
    {
        VkResult Result;
        uint32_t ImageIndex;
    };

    VulkanSwapchain(std::shared_ptr<VulkanDevice> device, VkSurfaceKHR surface, const VulkanSwapchainPreferences& preferences);
    ~VulkanSwapchain();
    
    static std::shared_ptr<VulkanSwapchain> Create(std::shared_ptr<VulkanDevice> device, VkSurfaceKHR surface, const VulkanSwapchainPreferences& preferences);
    
    std::shared_ptr<VulkanDevice> GetDevice() const { return m_Device; }
    VkSwapchainKHR GetHandle() const { return m_Swapchain; }
    VkSurfaceFormatKHR GetSurfaceFormat() const { return m_SurfaceFormat; }
    VkPresentModeKHR GetPresentMode() const { return m_PresentMode; }
    VkExtent2D GetExtent() const { return m_Extent; }
    uint32_t GetImageCount() const { return m_ImageCount; }
    uint32_t QueryImageCount() const;
    std::vector<std::shared_ptr<VulkanSwapchainImage>> GetSwapchainImages() const;

    AcquisitionResult AcquireNextImage(const VulkanSemaphore* semaphore = nullptr) const;

private:
    VkSurfaceFormatKHR SelectSurfaceFormat(const VkSurfaceKHR surface, const VkSurfaceFormatKHR& preference) const;
    VkPresentModeKHR SelectPresentMode(const VkSurfaceKHR surface, const VkPresentModeKHR preference) const;
    VkExtent2D SelectExtent(const VkSurfaceKHR surface, const VulkanSwapchainPreferences& preferences) const;
    uint32_t FigureImageCount(const VkSurfaceKHR surface, const VulkanSwapchainPreferences& preferences) const;
    void FetchSwapchainImages();

private:
    VkSwapchainKHR m_Swapchain;
    VkSurfaceFormatKHR m_SurfaceFormat;
    VkPresentModeKHR m_PresentMode;
    VkExtent2D m_Extent;
    uint32_t m_ImageCount;

    std::shared_ptr<VulkanDevice> m_Device;
    std::vector<std::shared_ptr<VulkanSwapchainImage>> m_SwapchainImages;
};  