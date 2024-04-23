#include "VulkanPipeline.hpp"

#include "VulkanPipelineLayout.hpp"

VulkanPipeline::VulkanPipeline
(
    VkPipelineBindPoint bindPoint,
    std::shared_ptr<VulkanDevice> device,
    std::shared_ptr<VulkanPipelineLayout> layout
) : m_Pipeline(VK_NULL_HANDLE), m_Device(device),
    m_BindPoint(bindPoint), m_Layout(layout)
{
}

VulkanPipeline::~VulkanPipeline()
{
    vkDestroyPipeline(m_Device->GetHandle(), m_Pipeline, nullptr);
}

VkPipeline VulkanPipeline::GetHandle() const 
{
    return m_Pipeline;
}

std::shared_ptr<VulkanDevice> VulkanPipeline::GetDevice() const
{
    return m_Device;
}