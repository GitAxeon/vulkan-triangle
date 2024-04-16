#include "VulkanPipelineLayout.hpp"

#include <Vulkan/vulkan.hpp>

VulkanPipelineLayout::VulkanPipelineLayout(std::shared_ptr<VulkanDevice> device, const VkPipelineLayoutCreateInfo& createInfo)
{
    VkResult result = vkCreatePipelineLayout(device->GetHandle(), &createInfo, nullptr, &m_PipelineLayout);

    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to create pipeline layout");
        throw std::runtime_error("Vulkan error");
    }
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
    vkDestroyPipelineLayout(m_Device->GetHandle(), m_PipelineLayout, nullptr);
}

VkPipelineLayout VulkanPipelineLayout::GetHandle() const
{
    return m_PipelineLayout;
}
