#pragma once

#include "VulkanDevice.hpp"

#include <Vulkan/vulkan.hpp>

#include <vector>

class VulkanShaderModule
{
public:
    VulkanShaderModule(std::shared_ptr<VulkanDevice> device, const std::vector<char>& bytes);
    ~VulkanShaderModule();

    static std::unique_ptr<VulkanShaderModule> Create(std::shared_ptr<VulkanDevice> device, const std::vector<char>& bytes); 

    VkShaderModule GetHandle() const;

private:
    std::shared_ptr<VulkanDevice> m_Device;
    VkShaderModule m_ShaderModule;
};