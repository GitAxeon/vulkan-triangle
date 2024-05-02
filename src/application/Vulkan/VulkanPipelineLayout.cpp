#include "VulkanPipelineLayout.hpp"

#include <Vulkan/vulkan.hpp>

VulkanPipelineLayout::VulkanPipelineLayout(std::shared_ptr<VulkanDevice> device, const VkPipelineLayoutCreateInfo& createInfo)
    : m_Device(device), m_PipelineLayout(VK_NULL_HANDLE)
{
    VkResult result = vkCreatePipelineLayout(device->GetHandle(), &createInfo, nullptr, &m_PipelineLayout);

    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to create pipeline layout");
        throw std::runtime_error("Vulkan error");
    }

    Log.Info("PipelineLayout created");
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
    vkDestroyPipelineLayout(m_Device->GetHandle(), m_PipelineLayout, nullptr);
    Log.Info("PipelineLayout destructed");
}

VkPipelineLayout VulkanPipelineLayout::GetHandle() const
{
    return m_PipelineLayout;
}
