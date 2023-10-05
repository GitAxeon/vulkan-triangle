#pragma once

#include "VulkanDeviceSelector.hpp"

#include <map>

class VulkanDevice
{
public:
    VulkanDevice(std::shared_ptr<VulkanDeviceSelector> deviceSelector)
        : m_Instance(deviceSelector->GetInstance()), m_PhysicalDevice(VK_NULL_HANDLE), m_Device(VK_NULL_HANDLE)
    {
        m_PhysicalDevice = deviceSelector->SelectDevice();

        auto requests = deviceSelector->GetRequests();

        Log.Info("Combining requests into FamilyInfo");
        auto familyInfos = CombineRequets(requests);

        Log.Info("Generating QueueCreateInfos based on familyInfos");
        auto createInfos = GenerateCreateInfos(familyInfos, requests);

        // Using default values for now
        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = createInfos.data();
        createInfo.queueCreateInfoCount = createInfos.size();
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

    VkQueue GetQueue(const VulkanQueueRequest& request, int queueIndex)
    {
        if(queueIndex >= request.QueueLocations.size())
            return VK_NULL_HANDLE;
        
        QueueLocation loc = request.QueueLocations[queueIndex];

        VkQueue queue;
        vkGetDeviceQueue(m_Device, loc.FamilyIndex, loc.Index, &queue);
        
        return queue;
    }

private:
    struct QueueFamilyInfo
    {
        QueueFamilyInfo() : QueueCount(0), QueuePriorities() {}

        int QueueCount;
        std::vector<float> QueuePriorities;
    };

    std::map<uint32_t, QueueFamilyInfo> CombineRequets(std::vector<VulkanQueueRequest>& requests)
    {
        std::map<uint32_t, QueueFamilyInfo> result;
        
        for(auto& request : requests)
        {
            size_t priorityIndex = 0;

            for(auto& queueLocation : request.QueueLocations)
            {
                uint32_t familyIndex = queueLocation.FamilyIndex;

                auto it = result.find(familyIndex);

                if (it == result.end())
                {
                    result.insert({familyIndex, {}});
                }

                QueueFamilyInfo& queueFamilyInfo = result[familyIndex];
                queueFamilyInfo.QueueCount++;

                uint32_t queueIndex = queueLocation.Index;

                if (queueFamilyInfo.QueuePriorities.size() < queueIndex + 1)
                {
                    queueFamilyInfo.QueuePriorities.resize(queueIndex + 1);
                }

                queueFamilyInfo.QueuePriorities[queueIndex] = request.Priorities[priorityIndex];
                
                priorityIndex++;
            }
        }

        return result;
    }

    std::vector<VkDeviceQueueCreateInfo> GenerateCreateInfos(std::map<uint32_t, QueueFamilyInfo>& familyInfos, std::vector<VulkanQueueRequest>& requests)
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

private:
    std::shared_ptr<VulkanInstance> m_Instance;
    std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
    VkDevice m_Device;
};