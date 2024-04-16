#pragma once

#include "VulkanDevice.hpp"

#include <Vulkan/vulkan.hpp>

#include <vector>

class VulkanShaderModule
{
public:
    VulkanShaderModule(std::shared_ptr<VulkanDevice> device, const std::vector<char>& bytes);
    ~VulkanShaderModule();

    VkShaderModule GetHandle() const;

private:
    std::shared_ptr<VulkanDevice> m_Device;
    VkShaderModule m_ShaderModule;
};