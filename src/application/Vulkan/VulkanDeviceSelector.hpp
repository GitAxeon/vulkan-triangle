#pragma once

#include "VulkanPhysicalDevice.hpp"

#include <Vulkan/vulkan.hpp>

#include <algorithm>
#include <map>
#include <bitset>

struct VulkanQueueFamilyInfo
{
    VkQueueFamilyProperties Properties;

    // consists of indices to m_Requests vector. (Present support was requested and can be provided by the queue family)
    std::set<uint32_t> PresentSupport;
    uint32_t Index;
};

struct VulkanDeviceInfo
{
    VkPhysicalDeviceProperties Properties;
    VkPhysicalDeviceFeatures Features;
    
    std::vector<VulkanQueueFamilyInfo> QueueFamilies;
};

class VulkanDeviceSelector
{
public:
    VulkanDeviceSelector(std::shared_ptr<VulkanInstance> instance, std::vector<VulkanQueueRequest>& queueRequests)
        : m_Instance(instance), m_Requests(queueRequests)
    {
        m_DeviceInfos = CollectDeviceInformation(instance, queueRequests);
    }

    std::shared_ptr<VulkanPhysicalDevice> SelectDevice()
    {
        VkPhysicalDevice result = VK_NULL_HANDLE;

        Log.Info("Device info");
        for(auto deviceInfo : m_DeviceInfos)
        {
            Log.Info(deviceInfo.Properties.deviceName, " ", deviceInfo.Properties.deviceType);
            
            for(auto family : deviceInfo.QueueFamilies)
            {
                Log.Info("  { ",
                    (family.Properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) ? "Graphics " :"",
                    (family.Properties.queueFlags & VK_QUEUE_TRANSFER_BIT) ? "Transfer " : "",
                    (family.Properties.queueFlags & VK_QUEUE_COMPUTE_BIT) ? "Compute " : "",
                    (family.Properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) ? "Sparse " : "",
                    (family.Properties.queueFlags & VK_QUEUE_PROTECTED_BIT) ? "Protected " : "",
                " }  Max Queues: ", family.Properties.queueCount);
            }
        }
        Log.Info("-------------");

        Thinking();

        for(auto device : VulkanPhysicalDevice::EnumeratePhysicalDevices(m_Instance))
        {
            DeviceQueueIndices indices = FindIndices(device);

            if(indices.Valid(m_Requests))
            {
                Log.Info("Suitable device found");
                return std::make_shared<VulkanPhysicalDevice>(m_Instance, device, indices);
            }
        }

        Log.Error("Queue requests couldn't be fulfilled");
        throw std::runtime_error("Failed to find suitable physical device");
    }

    static std::vector<VulkanDeviceInfo> CollectDeviceInformation(
        std::shared_ptr<VulkanInstance> instance,
        const std::vector<VulkanQueueRequest>& requests
    )
    {
        std::vector<VulkanDeviceInfo> deviceInfos;

        for(auto device : VulkanPhysicalDevice::EnumeratePhysicalDevices(instance))
        {
            VulkanDeviceInfo deviceInfo;
            deviceInfo.Properties = VulkanPhysicalDevice::GetDeviceProperties(device);
            deviceInfo.Features = VulkanPhysicalDevice::GetDeviceFeatures(device);
            
            uint32_t familyIndex = 0;
            for(auto queueFamily : VulkanPhysicalDevice::EnumerateDeviceQueueFamilyProperties(device))
            {
                VulkanQueueFamilyInfo queueFamilyInfo;
                queueFamilyInfo.Properties = queueFamily;
                queueFamilyInfo.Index = familyIndex;

                uint32_t requestIndex = 0;
                for(auto request : requests)
                {
                    if(request.Surface.has_value() && CheckSurfaceSupport(device, familyIndex, request.Surface.value()))
                    {
                        queueFamilyInfo.PresentSupport.insert(requestIndex);
                    }

                    ++requestIndex;
                }
                
                deviceInfo.QueueFamilies.push_back(queueFamilyInfo);
                ++familyIndex;
            }

            deviceInfos.push_back(deviceInfo);
        }

        return deviceInfos;
    }

    const std::vector<VulkanDeviceInfo>& GetDeviceInformation() const
    {
        return m_DeviceInfos;
    }

private:

    DeviceQueueIndices FindIndices(VkPhysicalDevice device)
    {
        DeviceQueueIndices queueIndices;

        for(auto request : m_Requests)
        {
            int familyIndex = 0;
            for(auto queueFamily : VulkanPhysicalDevice::EnumerateDeviceQueueFamilyProperties(device))
            {                
                // Doesn't support required operations
                if((queueFamily.queueFlags & request.Flags) != request.Flags)
                    continue;
                
                // Doesn't support present operations
                if(request.Surface.has_value())
                {
                    if(!CheckSurfaceSupport(device, familyIndex, request.Surface.value()))
                        continue;
                }
                    
                queueIndices.Indices.insert(std::make_pair(request.Flags, familyIndex));
                break;
                
                ++familyIndex;
            }
        }

        return queueIndices;
    }

    static bool CheckSurfaceSupport(VkPhysicalDevice device, uint32_t familyIndex, VkSurfaceKHR surface)
    {
        VkBool32 surfaceSupport = false;
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(device, familyIndex, surface, &surfaceSupport);
        
        if(result != VK_SUCCESS)
        {
            Log.Error("Failed to query device surface support");
            throw std::runtime_error("Vulkan did an oopsie");
        }

        return surfaceSupport;
    }

// Unused
    unsigned int CountQueueFamilyTypes(VkQueueFlags flags) const
    {
        unsigned int count = 0;

        if (flags & VK_QUEUE_GRAPHICS_BIT) count++;
        if (flags & VK_QUEUE_COMPUTE_BIT) count++;
        if (flags & VK_QUEUE_TRANSFER_BIT) count++;
        if (flags & VK_QUEUE_SPARSE_BINDING_BIT) count++;

        return count;   
    }

    std::vector<VulkanQueueRequest> CleanRequests(std::vector<VulkanQueueRequest> requests)
    {
        std::vector<VulkanQueueRequest> cleaned;

        for(auto& requirement :  requests)
        {
            auto result = std::find_if(cleaned.begin(), cleaned.end(), [&](VulkanQueueRequest value)
            {
                return requirement.Flags == value.Flags;
            });

            if(result == cleaned.end())
            {
                cleaned.emplace_back(requirement);
            }
            else
            {
                result->Count += requirement.Count;
                result->Priorities.insert(result->Priorities.begin(), requirement.Priorities.begin(), requirement.Priorities.end());
            }
        }

        return cleaned;
    }

    void Thinking()
    {
        struct FamilyData
        {
            VkQueueFlags Flags;
            uint32_t Count;
            std::vector<std::vector<VulkanQueueFamilyInfo>::iterator> Iterators;
        };

        std::vector<std::vector<FamilyData>> perDeviceQueueData;
        
        size_t deviceIndex = 0;
        for(auto& device : m_DeviceInfos)
        {
            std::vector<FamilyData> deviceQueueData;
            auto familyIterator = device.QueueFamilies.begin();

            for(; familyIterator != device.QueueFamilies.end(); ++familyIterator)
            {
                auto& family = *familyIterator;

                auto result = std::find_if(deviceQueueData.begin(), deviceQueueData.end(), [&](const FamilyData& val)
                {
                    return val.Flags == family.Properties.queueFlags;
                });
                
                if(result == deviceQueueData.end())
                {
                    FamilyData familyData;
                    familyData.Flags = family.Properties.queueFlags;
                    familyData.Count = family.Properties.queueCount;
                    Log.Info(" - ", FlagsToString(familyData.Flags), " : ", familyData.Count);
                    familyData.Iterators.push_back(familyIterator);

                    deviceQueueData.emplace_back(familyData);
                }
                else
                {
                    Log.Info("  AAAAAAAAAAAAAAAAAAAAAAA Combining");
                    result->Count += family.Properties.queueCount;
                    result->Iterators.push_back(familyIterator);
                }
            }

            perDeviceQueueData.emplace_back(deviceQueueData);
            ++deviceIndex;
        }

        Log.Info("Available queue types and counts");
        for(auto& family : perDeviceQueueData)
        {
            for(auto& queue : family)
            {
                Log.Info(FlagsToString(queue.Flags), ": ", queue.Count,  " | ", static_cast<uint32_t>(queue.Flags));
            }
        }

        auto cleanRequests = CleanRequests(m_Requests);

        Log.Info("Cleaned queue requests");
        for(auto& req : cleanRequests)
        {
            Log.Info(FlagsToString(req.Flags), " : ", req.Count);
        }
    }

// Unused
    bool DoesDeviceSupportRequests(const VulkanDeviceInfo& deviceInfo) const
    {
        size_t requestCount = m_Requests.size();

        // Each element represents the number of queues requested of that type which we will attempt to fulfill. ie. at the end it should all be zeros
        std::vector<uint32_t> unfulfilledRequestedQueueCount;

        for(auto& request : m_Requests)
        {
            unfulfilledRequestedQueueCount.push_back(request.Count);
        }
        
        // Each element represents the number of queues left of that type of family of queues
        std::vector<uint32_t> queueFamilyQueuesLeft;

        for(auto& family : deviceInfo.QueueFamilies)
        {
            queueFamilyQueuesLeft.push_back(family.Properties.queueCount);
        }

        // Attempt to fullfil each queue request by assigning queues from the physical device to match the requests
        for(size_t i = 0; i < requestCount; i++)
        {
            const VulkanQueueRequest& request = m_Requests[i];

            bool canBeFulfilled = false;

            for(auto& family : deviceInfo.QueueFamilies)
            {
                int requestedQueueTypeCount = CountQueueFamilyTypes(request.Flags);
                int supportedQueueTypeCount = CountQueueFamilyTypes(family.Properties.queueFlags);

                if(supportedQueueTypeCount < requestedQueueTypeCount)
                    continue;
                
                int extraFlags;
                if((request.Flags & family.Properties.queueFlags) == request.Flags)
                {

                }
            }
        }
    }

    std::string FlagsToString(VkQueueFlags flags)
    {
        std::bitset<32> bits(flags);

        Log.Info("flag bits: ", bits.to_string());

        std::string str;
        str.append("Mask: " + std::to_string(flags) + " { ");
        
        if(flags & VK_QUEUE_GRAPHICS_BIT)
            str.append("Graphics ");
        
        if(flags & VK_QUEUE_TRANSFER_BIT)
            str.append("Transfer ");
        
        if(flags & VK_QUEUE_COMPUTE_BIT)
            str.append("Compute ");
        
        if(flags & VK_QUEUE_SPARSE_BINDING_BIT)
            str.append("Sparse ");

        if(flags & VK_QUEUE_PROTECTED_BIT)
            str.append("Protected ");
            
        str.append("}");

        return str;
    }

// Unused
    int ScoreDevice(const VulkanDeviceInfo& deviceInfo) const
    {
        int score = 0;

        if(deviceInfo.Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 1000;
        
        auto result = DoesDeviceSupportRequests(deviceInfo);
    }

// Unused
    std::vector<int> ScoreDevices() const
    {
        std::vector<int> scores;

        for(auto& deviceInfo : m_DeviceInfos)
        {
            auto result = ScoreDevice(deviceInfo);

            scores.push_back(result);
        }

        return scores;
    }

private:
    std::shared_ptr<VulkanInstance> m_Instance;
    std::vector<VulkanQueueRequest> m_Requests;
    std::vector<VulkanDeviceInfo> m_DeviceInfos;
};