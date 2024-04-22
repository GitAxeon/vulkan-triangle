#include "VulkanPipeline.hpp"

VulkanPipeline::VulkanPipeline()
{

}

VulkanPipeline::~VulkanPipeline()
{

}

VkPipeline VulkanPipeline::GetHandle() const 
{
    return m_Pipeline;
}