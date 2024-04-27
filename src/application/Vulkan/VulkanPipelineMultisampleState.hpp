#pragma once

#include <Vulkan/vulkan.hpp>

class VulkanPipelineMultisampleState : public VkPipelineMultisampleStateCreateInfo
{
public:
    VulkanPipelineMultisampleState(
        uint8_t sampleCount,
        bool sampleShadingEnable,
        float minSampleShading = 1.0f,
        const VkSampleMask* pSampleMask = VK_NULL_HANDLE,
        bool alphaToCoverageEnable = false,
        bool alphaToOneEnable = false
    );

};