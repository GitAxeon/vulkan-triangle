#pragma once

#include "VulkanDeviceSelector.hpp"

class VulkanDevice
{
public:
    VulkanDevice(std::shared_ptr<VulkanDeviceSelector> deviceSelector)
        : m_Instance(deviceSelector->GetInstance()), m_PhysicalDevice(VK_NULL_HANDLE), m_Device(VK_NULL_HANDLE)
    {
        m_PhysicalDevice = deviceSelector->SelectDevice();

        auto requests = deviceSelector->GetRequests();
        auto queueCreateInfos = GenerateCreateInfos(requests);

        // Using default values for now
        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = 0;
        
        std::vector<const char*> ValidationLayers = m_Instance->GetEnabledLayers();

        if(m_Instance->Debugging())
        {
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

    ~VulkanDevice()
    {
        vkDestroyDevice(m_Device, nullptr);
        Log.Info("Destructed VulkanDevice");
    }

    VkQueue GetQueue(const VulkanQueueRequest& request, int index)
    {
        VkQueue queue;

        if(request.Indices[0].Count > index)
            vkGetDeviceQueue(m_Device, request.Indices[0].Index, index, &queue);


        return queue;
    } 

private:
    std::vector<VkDeviceQueueCreateInfo> GenerateCreateInfos(std::vector<VulkanQueueRequest>& requests)
    {
        std::vector<VkDeviceQueueCreateInfo> result;

        for(size_t i = 0; i < requests.size(); i++)
        {
            VulkanQueueRequest& request = requests[i];

            uint32_t priorityOffset = 0;
            
            for(auto& data : request.Indices)
            {
                VkDeviceQueueCreateInfo queueCreateInfo{};
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = data.Index;
                queueCreateInfo.queueCount = data.Count;

                queueCreateInfo.pQueuePriorities = &(requests[i].Priorities[priorityOffset]);

                priorityOffset += data.Count;
                result.emplace_back(queueCreateInfo);
            }
        }

        return result;
    }

private:
    std::shared_ptr<VulkanInstance> m_Instance;
    std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
    VkDevice m_Device;
};