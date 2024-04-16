#include "VulkanShaderModule.hpp"

VulkanShaderModule::VulkanShaderModule(std::shared_ptr<VulkanDevice> device, const std::vector<char>& bytes)
    : m_Device(device), m_ShaderModule(VK_NULL_HANDLE)
{
    VkShaderModuleCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = bytes.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(bytes.data());
    
    VkResult result = vkCreateShaderModule(device->GetHandle(), &createInfo, nullptr, &m_ShaderModule);

    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to create VkShaderModule");
        throw std::runtime_error("Vulkan error");
    }
}

VulkanShaderModule::~VulkanShaderModule()
{
    vkDestroyShaderModule(m_Device->GetHandle(), m_ShaderModule, nullptr);
}

VkShaderModule VulkanShaderModule::GetHandle() const
{
    return m_ShaderModule;
}