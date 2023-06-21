#pragma once

#include "../Window.hpp"

class RenderingContext
{
public:
    RenderingContext(const Window& window, const std::shared_ptr<VulkanInstance>& instance)
    {
        SDL_bool result = SDL_Vulkan_CreateSurface(window.GetNativeWindow(), instance->GetInstance(), &m_Surface);

        if(result == SDL_FALSE)
            throw std::runtime_error("Failed to create rendering context");

        m_VulkanInstance = instance;
    }

    ~RenderingContext()
    {
        vkDestroySurfaceKHR(m_VulkanInstance->GetInstance(), m_Surface, nullptr);
    }

public:
    std::shared_ptr<VulkanInstance> m_VulkanInstance;
    VkSurfaceKHR m_Surface;
};