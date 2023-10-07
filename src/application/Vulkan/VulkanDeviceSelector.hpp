#pragma once

#include "VulkanPhysicalDevice.hpp"

#include <Vulkan/vulkan.hpp>

#include <algorithm>
#include <map>
#include <bitset>

struct VulkanDeviceRequirements
{
    std::vector<VulkanQueueRequest> Queues;
    std::vector<std::string> Extensions;
};

class VulkanDeviceSelector
{
public:
    VulkanDeviceSelector(std::shared_ptr<VulkanInstance> vulkanInstance, const VulkanDeviceRequirements& deviceRequirements)
        : m_Instance(vulkanInstance)
    {
        std::vector<VulkanPhysicalDevice> physicalDevices;


        // Maybe instance should do this part and just return a vector of VulkanPhysicalDevice since the instance sorta owns everything?
        uint32_t physicalDeviceIndex = 0;
        for(auto physicalDeviceHandle : VulkanPhysicalDevice::EnumeratePhysicalDevices(vulkanInstance))
        {
            physicalDevices.emplace_back(vulkanInstance, physicalDeviceHandle, physicalDeviceIndex);
            physicalDeviceIndex++;
        }

        // Maybe the device Requirements can do this part? 
        for(auto& request : m_Requests)
            while(request.Priorities.size() <= request.Count)
                request.Priorities.push_back(1.0f);
    }

    VulkanDeviceSelector(std::shared_ptr<VulkanInstance> instance, std::vector<std::string>& requiredDeviceExtensions, std::vector<VulkanQueueRequest>& queueRequests)
        : m_Instance(instance), m_RequiredDeviceExtensions(requiredDeviceExtensions), m_Requests(queueRequests)
    {
        m_DeviceInfos = CollectDeviceInformation(instance, requiredDeviceExtensions, m_Requests);

        for(auto& request : m_Requests)
            while(request.Priorities.size() <= request.Count)
                request.Priorities.push_back(1.0f);
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
                Log.Info( StandardFlagsToString(family.Properties.queueFlags), " Max Queues: ", family.Properties.queueCount);
            }
        }
        Log.Info("-------------");

        struct DeviceScore
        {
            uint32_t DeviceIndex;
            int Value;
        };
        
        std::vector<DeviceScore> scores;

        size_t deviceIndex = 0;
        for(auto& info : m_DeviceInfos)
        {
            DeviceScore score;
            score.DeviceIndex = deviceIndex;
            score.Value = ScoreDevice(info); 
            
            scores.push_back(score);

            deviceIndex++;
        }

        // Sort device scores in descending order
        std::sort(scores.begin(), scores.end(), [](const DeviceScore& lhs, const DeviceScore& rhs)
        {
            return lhs.Value < rhs.Value;
        });

        DeviceScore bestDevice = scores[0];

        if(bestDevice.Value < 0)
        {
            Log.Error("Queue requests couldn't be fulfilled");
            throw std::runtime_error("Failed to find suitable physical device");
        }

        return std::make_shared<VulkanPhysicalDevice>(m_Instance, VulkanPhysicalDevice::GetDevice(m_Instance, bestDevice.DeviceIndex));
    }

    static std::vector<VulkanDeviceInfo> CollectDeviceInformation(
        std::shared_ptr<VulkanInstance> instance,
        std::vector<std::string>& requiredDeviceExtensions,
        const std::vector<VulkanQueueRequest>& requests
    )
    {
        std::vector<VulkanDeviceInfo> deviceInfos;
        
        uint32_t deviceIndex = 0;
        for(auto device : VulkanPhysicalDevice::EnumeratePhysicalDevices(instance))
        {
            VulkanDeviceInfo deviceInfo;
            deviceInfo.Index = deviceIndex;
            deviceInfo.Properties = VulkanPhysicalDevice::GetDeviceProperties(device);
            deviceInfo.Features = VulkanPhysicalDevice::GetDeviceFeatures(device);
            deviceInfo.ExtensionProperties = VulkanPhysicalDevice::GetDeviceExtensions(device);

            Log.Info("Supported device extensions:");
            size_t index = 0;
            for(auto& extensionProperties : deviceInfo.ExtensionProperties)
            {
                deviceInfo.Extensions.push_back(extensionProperties.extensionName);
                Log.Info("    ", index, ": ", extensionProperties.extensionName);
                index++;
            }
            Log.Info("End of extensions");
            
            uint32_t familyIndex = 0;
            for(auto queueFamily : VulkanPhysicalDevice::EnumerateDeviceQueueFamilyProperties(device))
            {
                VulkanQueueFamilyInfo queueFamilyInfo;
                queueFamilyInfo.Properties = queueFamily;
                queueFamilyInfo.Index = familyIndex;

                // To figure wether the queue family supports present operations we loop through requests  to find one that needs present support
                // Assuming that if the queue supports one surface it will support all surfaces
                for(auto request : requests)
                {
                    if(request.Surface.has_value() && CheckSurfaceSupport(device, familyIndex, request.Surface.value()))
                    {
                        queueFamilyInfo.PresentSupport = true;
                        break;
                    }
                }
                
                deviceInfo.QueueFamilies.push_back(queueFamilyInfo);
                ++familyIndex;
            }

            deviceInfos.push_back(deviceInfo);
            deviceIndex++;
        }

        return deviceInfos;
    }

    const std::vector<VulkanDeviceInfo>& GetDeviceInformation() const
    {
        return m_DeviceInfos;
    }

    std::shared_ptr<VulkanInstance> GetInstance() const
    {
        return m_Instance;
    }

    std::vector<VulkanQueueRequest> GetRequests() const
    {
        return m_Requests;
    }

    std::vector<VulkanQueueRequest> GetRequests()
    {
        return m_Requests;
    }

private:

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

    // Return the number of enabled flags
    unsigned int CountQueueFamilyFlags(VkQueueFlags flags) const
    {
        std::bitset<32> set(flags);

        return set.count();   
    }

    // return true if flags1 are present in flags2
    bool FlagsArePresent(VkQueueFlags flags1, VkQueueFlags flags2) const
    {
        return ((flags2 & flags1) == flags1);
    }   

    // Return all families containing atleast |request.Flags| flags (ascending order)
    std::vector<VulkanQueueFamilyInfo> FindSuitableFamilies(VulkanDeviceInfo& deviceInfo, const VulkanQueueRequest& request)
    {
        std::vector<VulkanQueueFamilyInfo> result;

        std::copy_if(deviceInfo.QueueFamilies.begin(), deviceInfo.QueueFamilies.end(), std::back_inserter(result), [&](const VulkanQueueFamilyInfo& info)
        {
            return FlagsArePresent(request.Flags, info.Properties.queueFlags) && DoesDeviceSupportExtensions(deviceInfo);
        });

        std::sort(result.begin(), result.end(), [&](const VulkanQueueFamilyInfo& lhs, const VulkanQueueFamilyInfo& rhs)
        {
            return CountQueueFamilyFlags(lhs.Properties.queueFlags) < CountQueueFamilyFlags(rhs.Properties.queueFlags);
        });

        return result;
    }

    bool DoesDeviceSupportExtensions(VulkanDeviceInfo& deviceInfo)
    {
        if(m_RequiredDeviceExtensions.size() == 0)
            return true;

        Log.Info("Checking whether the device supports required extensions");
        std::set<std::string> required(m_RequiredDeviceExtensions.begin(), m_RequiredDeviceExtensions.end());

        Log.Info("Requested extensions");
        for(auto ext : required)
        {
            Log.Info("    ", ext);
        }
        
        Log.Info("Following extensions are available");
        for(auto extension : deviceInfo.Extensions)
        {
            if(required.erase(extension) > 0)
            {
                Log.Info("    ", extension);
                deviceInfo.EnabledExtensions.push_back(extension);
            }
        }

        if(required.size() > 0)
        {
            Log.Info("Not all required device extensions were available");
            for(auto ext : required)
            {
                Log.Info("    ", ext);
            }
        }

        return required.empty();
    }

    // Returns true if the device can provide all requested queues
    bool DoesDeviceSupportRequests(VulkanDeviceInfo& deviceInfo)
    {
        size_t requestCount = m_Requests.size();

        // Each element represents the number of queues requested of that type which we will attempt to fulfill. ie. at the end it should all be zeros
        std::vector<int> unfulfilledRequestedQueueCount;

        for(auto& request : m_Requests)
        {
            unfulfilledRequestedQueueCount.push_back(request.Count);
        }
        
        // Each element represents the number of queues left at that family
        std::vector<int> familyQueuesLeft;

        for(auto& family : deviceInfo.QueueFamilies)
        {
            familyQueuesLeft.push_back(family.Properties.queueCount);
        }

        // Attempt to fullfil each queue request by assigning queues from the physical device to match the requests
        for(size_t i = 0; i < requestCount; i++)
        {
            VulkanQueueRequest& request = m_Requests[i];

            auto suitableFamilies = FindSuitableFamilies(deviceInfo, request);

            for(auto family : suitableFamilies)
            {
                if(familyQueuesLeft[family.Index] == 0)
                {
                    Log.Info("Family queues hit zero");
                    continue;
                }
                    
                int count = std::min(familyQueuesLeft[family.Index], unfulfilledRequestedQueueCount[i]);
                
                if(count == 0)
                {
                    break;
                }

                for (size_t i = 0; i < count; i++)
                {
                    // calculate new index to be used within a family
                    uint32_t index = family.Properties.queueCount - familyQueuesLeft[family.Index] + i;

                    request.QueueLocations.emplace_back(family.Index, index);
                }

                // These could be done inside the for above but I honestly can't be asked to do that rn
                unfulfilledRequestedQueueCount[i] -= count;
                familyQueuesLeft[family.Index] -= count;
            }

            if(unfulfilledRequestedQueueCount[i] > 0)
            {
                Log.Info("Not all requested queues could be provided");
                return false;
            }
        }

        return true;
    }

    std::string StandardFlagsToString(VkQueueFlags flags)
    {
        std::string str("{ ");
        
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

    VkQueueFlags SanitizeQueueFlags(VkQueueFlags flags)
    {
        const VkQueueFlags validFlagMask = VK_QUEUE_GRAPHICS_BIT |
            VK_QUEUE_COMPUTE_BIT |
            VK_QUEUE_TRANSFER_BIT |
            VK_QUEUE_SPARSE_BINDING_BIT |
            VK_QUEUE_PROTECTED_BIT;

        return (flags & validFlagMask);
    }

// Unused
    int ScoreDevice(VulkanDeviceInfo& deviceInfo)
    {
        int score = 0;

        if(deviceInfo.Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 1000;
        
        bool result = DoesDeviceSupportRequests(deviceInfo);
        
        if(!result)
        {
            return -1;
        }

        return score;
    }

private:
    std::shared_ptr<VulkanInstance> m_Instance;
    std::vector<VulkanQueueRequest> m_Requests;
    std::vector<std::string> m_RequiredDeviceExtensions;
    std::vector<VulkanDeviceInfo> m_DeviceInfos;
};