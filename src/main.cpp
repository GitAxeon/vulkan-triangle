#include "application/sdl-wrap/SDLContextWrapper.hpp"
#include "application/Window.hpp"

#include "application/Vulkan/VulkanInstance.hpp"
#include "application/Vulkan/DebugUtilsMessenger.hpp"
#include "application/Vulkan/VulkanDeviceSelector.hpp"
#include "application/Vulkan/VulkanDevice.hpp"
#include "application/Vulkan/VulkanSwapchain.hpp"
#include "application/Vulkan/VulkanImageView.hpp"
#include "application/Vulkan/VulkanShaderModule.hpp"
#include "application/Vulkan/VulkanPipelineLayout.hpp"

#include "application/RenderingContext.hpp"

#include <SDL3/SDL.h>
#include <glm/vec2.hpp>
#include <glm/mat2x2.hpp>
#include <Vulkan/vulkan.hpp>

#include <fstream>

static std::vector<char> ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if(!file.is_open())
    {
        throw std::runtime_error("Failed to open " + filename + " for reading");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());

    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, 
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

void RunApplication();

int main(int argc, char* argv[])
{
    try
    {
        RunApplication();
    }
    catch(const std::exception& exception)
    {
        Log.Error(exception.what());
        
        return (EXIT_FAILURE);
    }
        
    return (EXIT_SUCCESS);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, 
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    const char* msg = "Vulkan validation layer: ";

    switch(messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            Log.Error(msg, pCallbackData->pMessage, "\n");
        break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            Log.Warn(msg, pCallbackData->pMessage, "\n");
        break;
        default: //VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            Log.Info(msg, pCallbackData->pMessage, "\n");
        break;
    }

    return VK_FALSE;
}

void RecordCommandBuffer(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer frameBuffer, VkExtent2D extent, VkPipeline pipeline)
{
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    VkResult commandBeginInfoResult = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if(commandBeginInfoResult != VK_SUCCESS)
    {
        Log.Error("Failed to begin recording command buffer");
        throw std::runtime_error("Vulkan error");
    }

    VkRenderPassBeginInfo renderPassBeginInfo {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = frameBuffer;
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor {};
    scissor.offset = {0, 0};
    scissor.extent = extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    VkResult endCommandBufferResult =  vkEndCommandBuffer(commandBuffer);

    if(endCommandBufferResult != VK_SUCCESS)
    {
        Log.Error("Failed to record command buffer end");
        throw std::runtime_error("Vulkan error");
    }
}

void RunApplication()
{
    WindowInfo info("Vulkan-triangle", 720, 300);

    SDLContextWrapper SDLContext;
    Log.Info("SDLContext Initialized");
    SDLContext.EnableVulkan();
    
    VulkanInstanceCreateInfo createInfo;
    createInfo.ApplicationName = info.Title;
    createInfo.EnableValidationLayers = true;
    createInfo.Extensions = SDLContext.GetVulkanInstanceExtensions();
    createInfo.Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    createInfo.ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

    std::shared_ptr<VulkanInstance> vulkanInstance = std::make_shared<VulkanInstance>(createInfo);
    
    DebugUtilsMessenger debugMessenger(vulkanInstance, DebugCallback);

    Window window(info);
    RenderingContext renderingContext(window, vulkanInstance);

    VulkanQueueRequest req1;
    req1.Flags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT;
    req1.Surface = renderingContext.GetSurface();
    req1.Count = 1;

    std::shared_ptr<VulkanDeviceRequirements> requirements = std::make_shared<VulkanDeviceRequirements>();
    requirements->Queues.push_back(req1);
    requirements->Extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    Log.Info("Creating device selector");
    std::shared_ptr<VulkanDeviceSelector> selector = std::make_shared<VulkanDeviceSelector>(vulkanInstance, requirements);
    
    Log.Info("Creating logical device");
    std::shared_ptr<VulkanDevice> device = selector->GetDevice();

    Log.Info("Requesting test queue");
    VkQueue graphicsQueue = device->GetQueue(requirements->Queues[0], 0);

    if(graphicsQueue == VK_NULL_HANDLE)
    {
        Log.Error("Invalid queue handle");
        throw std::runtime_error("Vulkan error");
    }

    VulkanSwapchainPreferences swapchainPreferences;
    swapchainPreferences.SurfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
    swapchainPreferences.SurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchainPreferences.PresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    swapchainPreferences.ImageCount = 2;
    swapchainPreferences.ImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    swapchainPreferences.SharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchainPreferences.QueueFamilyIndices = requirements->Queues[0].GetFamilyIndices();
    swapchainPreferences.CompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    std::shared_ptr<VulkanSwapchain> swapchain = std::make_shared<VulkanSwapchain>(device, renderingContext.GetSurface(), swapchainPreferences);

    std::vector<VkImage> swapchainImages = swapchain->GetSwapchainImages();
    
    Log.Info("Swapchain image count: ", swapchainImages.size());

    Log.Info("Creating ImageViews");
    std::vector<std::shared_ptr<VulkanImageView>> imageViews;
    for(VkImage swapImage : swapchainImages)
    {
        imageViews.emplace_back(std::make_shared<VulkanImageView>(swapchain, swapImage, device));
    }

    VkAttachmentDescription colorAttachment {};
    colorAttachment.format = swapchain->GetSurfaceFormat().format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPass renderPass;

    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkResult createRenderPassResult = vkCreateRenderPass(device->GetHandle(), &renderPassInfo, nullptr, &renderPass);

    if(createRenderPassResult != VK_SUCCESS)
    {
        Log.Error("Failed to create render pass");
        throw std::runtime_error("Vulkan error");
    }

    // Load shaders
    std::vector<char> vertexShaderCode = ReadFile("shaders/vert.spv");
    std::vector<char> fragmentShaderCode = ReadFile("shaders/frag.spv");

    VulkanShaderModule vertexShaderModule(device, vertexShaderCode);
    VulkanShaderModule fragmentShaderModule(device, fragmentShaderCode);

    VkPipelineShaderStageCreateInfo vertexShaderStageInfo {};
    vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageInfo.module = vertexShaderModule.GetHandle();
    vertexShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo {};
    fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageInfo.module = fragmentShaderModule.GetHandle();
    fragmentShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages [] = { vertexShaderStageInfo, fragmentShaderStageInfo };

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    VkExtent2D extent = swapchain->GetExtent();
    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = extent.width;
    viewport.height = extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor {};
    scissor.offset = {0 , 0};
    scissor.extent = extent;

    VkPipelineViewportStateCreateInfo viewportState {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // These are optional
    rasterizer.depthBiasClamp = 0.0f; // These are optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // These are optional

    VkPipelineMultisampleStateCreateInfo multisampling {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // These are optional
    multisampling.pSampleMask = nullptr; // These are optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // These are optional
    multisampling.alphaToOneEnable = VK_FALSE; // These are optional
    
    VkPipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // These are optional
    colorBlending.blendConstants[1] = 0.0f; // These are optional
    colorBlending.blendConstants[2] = 0.0f; // These are optional
    colorBlending.blendConstants[3] = 0.0f; // These are optional

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // These are optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // These are optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // These are optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // These are optional

    VulkanPipelineLayout pipelineLayout(device, pipelineLayoutInfo);

    VkGraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // This is optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout.GetHandle();
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // These are optional
    pipelineInfo.basePipelineIndex = -1; // These are optional

    VkPipeline graphicsPipeline;
    VkResult createPipelineResult = vkCreateGraphicsPipelines(device->GetHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);

    if(createPipelineResult != VK_SUCCESS)
    {
        Log.Error("Failed to create graphics pipeline");
        throw std::runtime_error("Vulkan error");
    }

    std::vector<VkFramebuffer> swapChainFramebuffers;
    swapChainFramebuffers.resize(imageViews.size());

    for(size_t i = 0; i < imageViews.size(); i++)
    {
        VkImageView attachments[] = {
            imageViews[i]->GetHandle()
        };

        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        VkResult createFramebufferResult = vkCreateFramebuffer(device->GetHandle(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
        
        if(createFramebufferResult != VK_SUCCESS)
        {
            Log.Error("Failed to create framebuffer");
            throw std::runtime_error("Vulkan error");
        }
    }

    VkCommandPool commandPool;
    
    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = *requirements->Queues[0].GetFamilyIndices().begin();

    VkResult createCommandPoolResult = vkCreateCommandPool(device->GetHandle(), &poolInfo, nullptr, &commandPool);

    if(createCommandPoolResult != VK_SUCCESS)
    {
        Log.Error("Failed to create commandpool");
        throw std::runtime_error("Vulkan error");
    }

    VkCommandBuffer commandBuffer;
    
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VkResult commandBufferCreateResult = vkAllocateCommandBuffers(device->GetHandle(), &allocInfo, &commandBuffer);

    if(commandBufferCreateResult != VK_SUCCESS)
    {
        Log.Error("Failed to create commandbuffer");
        throw std::runtime_error("Vulkan error");
    }

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    VkSemaphoreCreateInfo semaphoreCreateInfo {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult result = vkCreateSemaphore(device->GetHandle(), &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore);

    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to create semaphore");
        throw std::runtime_error("Vulkan error");
    }

    result = vkCreateSemaphore(device->GetHandle(), &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore);

    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to create semaphore");
        throw std::runtime_error("Vulkan error");
    }

    VkFenceCreateInfo fenceCreateInfo {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    result = vkCreateFence(device->GetHandle(), &fenceCreateInfo, nullptr, &inFlightFence);

    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to create fence");
        throw std::runtime_error("Vulkan error");
    }


    Log.Info("Entering EventLoop");
    while(window.IsOpen())
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    window.Close();
                break;
                case SDL_EVENT_KEY_DOWN:
                {
                    if(e.key.keysym.sym == SDLK_ESCAPE)
                        window.Close();
                } break;
            }
        }
        // Rendering thingies?

        vkWaitForFences(device->GetHandle(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device->GetHandle(), 1, &inFlightFence);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(device->GetHandle(), swapchain->GetHandle(),UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        vkResetCommandBuffer(commandBuffer, 0);

        RecordCommandBuffer(commandBuffer, renderPass, swapChainFramebuffers[imageIndex], extent, graphicsPipeline);

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VkResult submitResult = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence);

        if(submitResult != VK_SUCCESS)
        {
            Log.Error("Failed to submit draw command buffer");
            throw std::runtime_error("Vulkan error");
        }

        VkPresentInfoKHR presentInfo {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapchain->GetHandle()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        vkQueuePresentKHR(graphicsQueue, &presentInfo);
    }

    vkDeviceWaitIdle(device->GetHandle());

    vkDestroyPipeline(device->GetHandle(), graphicsPipeline, nullptr);

    for(auto framebuffer : swapChainFramebuffers)
    {
        vkDestroyFramebuffer(device->GetHandle(), framebuffer, nullptr);
    }

    vkDestroyCommandPool(device->GetHandle(), commandPool, nullptr);

    vkDestroySemaphore(device->GetHandle(), imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(device->GetHandle(), renderFinishedSemaphore, nullptr);
    vkDestroyFence(device->GetHandle(), inFlightFence, nullptr);

    Log.Info("Exiting EventLoop");
}