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
#include "application/Vulkan/VulkanFence.hpp"
#include "application/Vulkan/VulkanSemaphore.hpp"
#include "application/Vulkan/VulkanRenderPass.hpp"
#include "application/Vulkan/VulkanAttachmentDescription.hpp"
#include "application/Vulkan/VulkanAttachmentReference.hpp"
#include "application/Vulkan/VulkanSubpassDependency.hpp"
#include "application/Vulkan/VulkanSubpassDescription.hpp"
#include "application/Vulkan/VulkanRect2D.hpp"
#include "application/Vulkan/VulkanCommandPool.hpp"
#include "application/Vulkan/VulkanCommandBuffer.hpp"
#include "application/Vulkan/VulkanViewport.hpp"
#include "application/Vulkan/VulkanPipeline.hpp"
#include "application/Vulkan/VulkanPipelineShaderStage.hpp"
#include "application/Vulkan/VulkanPipelineDynamicState.hpp"
#include "application/Vulkan/VulkanPipelineVertexInputState.hpp"
#include "application/Vulkan/VulkanPipelineInputAssemblyState.hpp"
#include "application/Vulkan/VulkanPipelineViewportState.hpp"
#include "application/Vulkan/VulkanPipelineRasterizationState.hpp"
#include "application/Vulkan/VulkanPipelineMultisampleState.hpp"
#include "application/Vulkan/VulkanPipelineColorBlendAttachment.hpp"
#include "application/Vulkan/VulkanPipelineColorBlendState.hpp"
#include "application/Vulkan/VulkanGraphicsPipeline.hpp"
#include "application/Vulkan/VulkanPipelineDepthStencilState.hpp"
#include "application/Vulkan/VulkanQueue.hpp"

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
    catch(std::exception& exception)
    {
        Log.Error(exception.what());
        
        return (EXIT_FAILURE);
    }
    catch(...)
    {
        Log.Error("Unknown critical error");
    }

    Log.Info("End of main");
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

void RecordCommandBuffer
(
    std::shared_ptr<VulkanCommandBuffer> commandBuffer,
    std::shared_ptr<VulkanRenderPass> renderPass,
    std::shared_ptr<VulkanFramebuffer> frameBuffer,
    VkExtent2D extent,
    std::shared_ptr<VulkanPipeline> pipeline
)
{
    commandBuffer->Begin();
    
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    commandBuffer->BeginRenderPass(
        renderPass,
        frameBuffer,
        VulkanRect2D(0, 0, extent.width, extent.height),
        clearColor,
        VK_SUBPASS_CONTENTS_INLINE
     );

    commandBuffer->BindPipeline(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS);

    VulkanViewport viewport(0, 0, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, 1.0f);
    commandBuffer->SetViewport(viewport);

    VulkanRect2D scissor(extent);
    commandBuffer->SetScissor(scissor);

    commandBuffer->Draw(3, 0);

    commandBuffer->EndRenderPass();
    commandBuffer->End();
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
    std::shared_ptr<VulkanQueue> graphicsQueue = device->GetQueue(requirements->Queues[0], 0);

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

    std::vector<std::shared_ptr<VulkanSwapchainImage>> swapchainImages = swapchain->GetSwapchainImages();
    
    Log.Info("Swapchain image count: ", swapchainImages.size());

    std::vector<std::shared_ptr<VulkanImageView>> imageViews;
    for(std::shared_ptr<VulkanSwapchainImage> swapImage : swapchainImages)
    {
        imageViews.emplace_back(std::make_shared<VulkanImageView>(swapImage));
    }

    VulkanAttachmentDescription colorAttachment(
        swapchain->GetSurfaceFormat().format,
        VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    );

    std::shared_ptr<VulkanAttachmentReference> colorAttachmentReference = std::make_shared<VulkanAttachmentReference>(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    
    VulkanSubpassDescription subpass(
        colorAttachmentReference
    );

    VulkanSubpassDependency subpassDependency(
        VK_SUBPASS_EXTERNAL,
        0,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        0
    );

    std::shared_ptr<VulkanRenderPass> renderPass = std::make_shared<VulkanRenderPass>(device, colorAttachment, subpass, subpassDependency);

    // Load shaders
    std::vector<char> vertexShaderCode = ReadFile("shaders/vert.spv");
    std::vector<char> fragmentShaderCode = ReadFile("shaders/frag.spv");

    std::shared_ptr<VulkanShaderModule> vertexShaderModule = std::make_shared<VulkanShaderModule>(device, vertexShaderCode);
    std::shared_ptr<VulkanShaderModule> fragmentShaderModule = std::make_shared<VulkanShaderModule>(device, fragmentShaderCode);

    VulkanPipelineShaderStage vertexShaderStage(
        VK_SHADER_STAGE_VERTEX_BIT,
        vertexShaderModule
    );

    VulkanPipelineShaderStage fragmentShaderStage(
        VK_SHADER_STAGE_FRAGMENT_BIT,
        fragmentShaderModule
    );

    std::vector<VulkanPipelineShaderStage> shaderStages({vertexShaderStage, fragmentShaderStage});

    VulkanPipelineDynamicState dynamicStates({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR});
    VulkanPipelineVertexInputState vertexInput;

    VulkanPipelineInputAssemblyState inputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
    
    VkExtent2D extent = swapchain->GetExtent();

    VulkanViewport viewport(0, 0, extent.width, extent.height, 0, 1.0f);

    VulkanRect2D scissor(extent);

    VulkanPipelineViewportState viewportstate(viewport, scissor);

    VulkanPipelineRasterizationState rasterizationState(
        VK_POLYGON_MODE_FILL,
        VK_CULL_MODE_BACK_BIT,
        false,
        false,
        VK_FRONT_FACE_CLOCKWISE,
        false,
        1.0f     
    );
    
    VulkanPipelineMultisampleState multisample(1, false);

    VulkanPipelineColorBlendAttachment colorBlendAttachment(
        true,
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        VK_BLEND_FACTOR_SRC_ALPHA,
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ONE,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD
    );

    std::vector<VulkanPipelineColorBlendAttachment> colorblendAttachments = {colorBlendAttachment};

    VulkanPipelineColorBlendState colorBlendState(
        false,
        VK_LOGIC_OP_COPY,
        colorblendAttachments
    );

    VulkanPipelineDepthStencilState depthStencilState(
        true,
        true,
        VK_COMPARE_OP_LESS,
        false,
        false
    );

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // These are optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // These are optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // These are optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // These are optional

    std::shared_ptr<VulkanPipelineLayout> pipelineLayout = std::make_shared<VulkanPipelineLayout>(device, pipelineLayoutInfo);

    std::shared_ptr<VulkanGraphicsPipeline> graphicsPipeline = std::make_shared<VulkanGraphicsPipeline>
    (
        device,
        shaderStages,
        vertexInput,
        inputAssembly,
        viewportstate,
        rasterizationState,
        multisample,
        depthStencilState,
        colorBlendState,
        dynamicStates,
        pipelineLayout,
        renderPass,
        0
    );

    std::vector<std::shared_ptr<VulkanFramebuffer>> framebuffers;
    for(auto view : imageViews)
    {
        framebuffers.emplace_back(std::make_shared<VulkanFramebuffer>(renderPass, view));
    }

    std::shared_ptr<VulkanCommandPool> commandPool = std::make_shared<VulkanCommandPool>
    (
        device,
        *requirements->Queues[0].GetFamilyIndices().begin(),
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
    );

    std::shared_ptr<VulkanCommandBuffer> commandBuffer = commandPool->CreatePrimaryBuffer();

    // Synchronization
    std::shared_ptr<VulkanSemaphore> imageAvailableSem = std::make_shared<VulkanSemaphore>(device);
    std::shared_ptr<VulkanSemaphore> renderFinishedSem = std::make_shared<VulkanSemaphore>(device);

    std::shared_ptr<VulkanFence> inFlightFence = std::make_shared<VulkanFence>(device, VK_FENCE_CREATE_SIGNALED_BIT);

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
        
        inFlightFence->Wait();
        inFlightFence->Reset();

        uint32_t imageIndex = swapchain->AcquireNextImage(imageAvailableSem);
        commandBuffer->Reset();

        RecordCommandBuffer(commandBuffer, renderPass, framebuffers[imageIndex], extent, graphicsPipeline);

        graphicsQueue->Submit(
            commandBuffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            imageAvailableSem,
            renderFinishedSem,
            inFlightFence
        );

        graphicsQueue->Present(imageIndex, swapchain, renderFinishedSem);
    }
    
    device->WaitIdle();

    commandPool->DestroyCommandBuffer(commandBuffer);

    Log.Info("Exiting EventLoop");
}