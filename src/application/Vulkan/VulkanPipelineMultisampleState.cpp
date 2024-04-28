#include "VulkanPipelineMultisampleState.hpp"

VulkanPipelineMultisampleState::VulkanPipelineMultisampleState(
    uint8_t sampleCount,
    bool sampleShadingEnable,
    float minSampleShading,
    const VkSampleMask* pSampleMask,
    bool alphaToCoverageEnable,
    bool alphaToOneEnable
) : VkPipelineMultisampleStateCreateInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        VK_NULL_HANDLE,
        0,
        VK_SAMPLE_COUNT_1_BIT,
        sampleShadingEnable ? VK_TRUE : VK_FALSE,
        minSampleShading,
        pSampleMask,
        alphaToCoverageEnable ? VK_TRUE : VK_FALSE,
        alphaToOneEnable ? VK_TRUE : VK_FALSE
    }
{
    switch(sampleCount)
    {
    case 1:
        sampleCount = VK_SAMPLE_COUNT_1_BIT;
    break;
    case 2:
        sampleCount = VK_SAMPLE_COUNT_2_BIT;
    break;
    case 4:
        sampleCount = VK_SAMPLE_COUNT_4_BIT;
    break;
    case 8:
        sampleCount = VK_SAMPLE_COUNT_8_BIT;
    break;
    case 16:
        sampleCount = VK_SAMPLE_COUNT_16_BIT;
    break;
    case 32:
    default:
        sampleCount = VK_SAMPLE_COUNT_32_BIT;
    }
}