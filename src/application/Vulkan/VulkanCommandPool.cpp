#include "VulkanCommandPool.hpp"
#include "VulkanCommandBuffer.hpp"

VulkanCommandPool::VulkanCommandPool(std::shared_ptr<VulkanDevice> device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
    : m_Device(device), m_CommandPool(VK_NULL_HANDLE), m_QueueFamilyIndex(queueFamilyIndex)
{
    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = flags;
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    VkResult result = vkCreateCommandPool(device->GetHandle(), &poolInfo, nullptr, &m_CommandPool);

    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to create commandpool");
        throw std::runtime_error("Vulkan error");
    }

    Log.Info("CommandPool created");
}

VulkanCommandPool::~VulkanCommandPool()
{
    vkDestroyCommandPool(m_Device->GetHandle(), m_CommandPool, nullptr);
    Log.Info("CommandPool destructed");
}

VkCommandPool VulkanCommandPool::GetHandle() const
{
    return m_CommandPool;
}

std::shared_ptr<VulkanCommandBuffer> VulkanCommandPool::CreatePrimaryBuffer()
{
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer bufferHandle = VK_NULL_HANDLE;

    VkResult result = vkAllocateCommandBuffers(m_Device->GetHandle(), &allocInfo, &bufferHandle);

    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to create commandbuffer");
        throw std::runtime_error("Vulkan error");
    }

    Log.Info("Primary CommandBuffer created");
    
    std::shared_ptr<VulkanCommandBuffer> commandBuffer = std::make_shared<VulkanCommandBuffer>(shared_from_this(), bufferHandle);
    
    return commandBuffer;   
}

std::vector<std::shared_ptr<VulkanCommandBuffer>> VulkanCommandPool::CreatePrimaryBuffers(uint32_t bufferCount)
{
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = bufferCount;

    std::vector<VkCommandBuffer> bufferHandles(bufferCount);

    VkResult result = vkAllocateCommandBuffers(m_Device->GetHandle(), &allocInfo, bufferHandles.data());

    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to create commandbuffer");
        throw std::runtime_error("Vulkan error");
    }

    std::vector<std::shared_ptr<VulkanCommandBuffer>> commandBuffers(bufferCount);

    for(VkCommandBuffer bufferHandle : bufferHandles)
    {
        commandBuffers.emplace_back(std::make_shared<VulkanCommandBuffer>(shared_from_this(), bufferHandle));
    }

    return commandBuffers;
}

void VulkanCommandPool::DestroyCommandBuffer(std::shared_ptr<VulkanCommandBuffer> commandBuffer)
{
    VkCommandBuffer handle = commandBuffer->GetHandle();
    commandBuffer->m_CommandBuffer = VK_NULL_HANDLE;

    vkFreeCommandBuffers(m_Device->GetHandle(), m_CommandPool, 1, &handle);
    Log.Info("CommandBuffer destroyed");
    commandBuffer = nullptr;
}

void VulkanCommandPool::DestroyCommandBuffers(std::vector<std::shared_ptr<VulkanCommandBuffer>>& commandBuffers)
{
    std::vector<VkCommandBuffer> bufferHandles(commandBuffers.size());

    for(auto& commandBuffer : commandBuffers)
    {
        bufferHandles.emplace_back(commandBuffer->GetHandle());
        commandBuffer->m_CommandBuffer = VK_NULL_HANDLE;
    }

    vkFreeCommandBuffers(m_Device->GetHandle(), m_CommandPool, bufferHandles.size(), bufferHandles.data());
    
    commandBuffers.clear();
}

std::shared_ptr<VulkanDevice> VulkanCommandPool::GetDevice() const
{
    return m_Device;
}