#pragma once

#include "Window.hpp"
#include "Vulkan/VulkanSurface.hpp"

class RenderingContext
{
public:
    // Maybe overload the constructor to handle different type of windows?
    RenderingContext(const Window& window, std::shared_ptr<VulkanInstance> instance)
    {
        m_Surface = window.CreateSurface(instance);

        m_VulkanInstance = instance;
    }

    ~RenderingContext()
    {
        vkDestroySurfaceKHR(m_VulkanInstance->GetInstance(), m_Surface, nullptr);
    }

    VkSurfaceKHR GetSurface() const
    {
        return m_Surface;
    }

public:
    std::shared_ptr<VulkanInstance> m_VulkanInstance;
    VkSurfaceKHR m_Surface;
};