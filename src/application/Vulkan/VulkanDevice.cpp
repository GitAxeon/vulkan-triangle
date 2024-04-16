#include "VulkanDevice.hpp"
#include "VulkanInstance.hpp"

VulkanDevice::VulkanDevice(std::shared_ptr<VulkanPhysicalDevice> physicalDevice, std::shared_ptr<VulkanDeviceRequirements> requirements)
    : m_Instance(physicalDevice->GetInstance()), m_PhysicalDevice(physicalDevice)
{
    auto familyCreateInfos = requirements->CombineQueueRequestIntoQueueFamilyCreateInfos();
    auto createInfos = GenerateCreateInfos(familyCreateInfos);

    // Using default values for now
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = createInfos.data();
    createInfo.queueCreateInfoCount = createInfos.size();
    createInfo.pEnabledFeatures = &deviceFeatures;
    
    std::vector<std::string> enabledExtension = physicalDevice->GetEnabledExtensions();
    std::vector<const char*> cStrExtensions;

    if(enabledExtension.size() == 0)
    {
        createInfo.enabledExtensionCount = 0;
    }
    else
    {
        for(auto& extension : enabledExtension)
        {
            cStrExtensions.push_back(extension.c_str());
        }

        createInfo.enabledExtensionCount = static_cast<uint32_t>(cStrExtensions.size());
        createInfo.ppEnabledExtensionNames = cStrExtensions.data();
    }
    
    std::vector<const char*> ValidationLayers;

    if(m_Instance->Debugging())
    {
        ValidationLayers = m_Instance->GetEnabledLayers();

        createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
        createInfo.ppEnabledLayerNames = ValidationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    VkResult result = vkCreateDevice(m_PhysicalDevice->GetHandle(), &createInfo, nullptr, &m_Device);

    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to create Vulkan LogicalDevice");
        throw std::runtime_error("Graphics error");
    }

    Log.Info("Created VulkanDevice");
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(m_Device, nullptr);
    Log.Info("Destructed VulkanDevice");
}

const SwapchainSupportDetails VulkanDevice::GetSwapchainSupportDetails(VkSurfaceKHR surface)
{
    return m_PhysicalDevice->GetSwapchainSupportDetails(surface);
}


VkQueue VulkanDevice::GetQueue(const VulkanQueueRequest& request, int queueIndex)
{
    if(queueIndex >= request.QueueLocations.size())
        return VK_NULL_HANDLE;
    
    QueueLocation loc = request.QueueLocations[queueIndex];

    VkQueue queue;
    vkGetDeviceQueue(m_Device, loc.FamilyIndex, loc.Index, &queue);
    
    return queue;
}

std::vector<VkDeviceQueueCreateInfo> VulkanDevice::GenerateCreateInfos(std::map<uint32_t, QueueFamilyCreateInfo>& familyInfos)
{        
    std::vector<VkDeviceQueueCreateInfo> result;
    
    for(auto& [familyIndex, familyInfo] : familyInfos)
    {
        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = familyIndex;
        queueCreateInfo.queueCount = familyInfo.QueueCount;
        queueCreateInfo.pQueuePriorities = familyInfo.QueuePriorities.data();

        result.emplace_back(queueCreateInfo);
    }

    Log.Info("GenerateCreateInfos");
    for(auto value : result)
    {
        std::string message = "Family[" + std::to_string(value.queueFamilyIndex) +"] [";

        for(size_t i = 0; i < value.queueCount; i++)
        {
            message.append(std::to_string(value.pQueuePriorities[i]));

            if(i < value.queueCount - 1)
                message.append(", ");
        }
        message.append("]");
        
        Log.Info(message);
    }

    return result;
}