#include "VulkanDeviceSelector.hpp"
#include "VulkanInstance.hpp"

VulkanDeviceSelector::VulkanDeviceSelector(std::shared_ptr<VulkanInstance> vulkanInstance, VulkanDeviceRequirements& requirements)
    : m_Instance(vulkanInstance), m_DeviceRequirements(requirements)
{
    m_DeviceRequirements.FillQueuePrioritiesIfNeeded();

    auto physicalDevices = vulkanInstance->GetPhysicalDevices();

    for(auto device : physicalDevices)
    {
        Log.Info(device->GetProperties().deviceName, " ", device->GetProperties().deviceType);

        for(auto family : device->GetQueueFamilyInfos())
        {
            Log.Info(StandardFlagsToString(family.Properties.queueFlags), " Max Queues: ", family.Properties.queueCount);
        }
    }
    Log.Info("--------------");

    struct DeviceScore 
    {
        uint32_t DeviceIndex;
        int Value;
    };

    std::vector<DeviceScore> scores;
    
    size_t deviceIndex = 0;
    for(auto& device : physicalDevices)
    {
        DeviceScore score;
        score.DeviceIndex = deviceIndex;
        score.Value = ScoreDevice(device); 
        
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

    m_Device = std::make_shared<VulkanDevice>(physicalDevices[bestDevice.DeviceIndex], m_DeviceRequirements);
}

std::shared_ptr<VulkanDevice> VulkanDeviceSelector::GetDevice()
{
    return m_Device;
}

bool VulkanDeviceSelector::CheckSurfaceSupport(std::shared_ptr<VulkanPhysicalDevice> physicalDevice, uint32_t familyIndex, VkSurfaceKHR surface)
{
    VkBool32 surfaceSupport = false;
    VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice->GetHandle(), familyIndex, surface, &surfaceSupport);
    
    if(result != VK_SUCCESS)
    {
        Log.Error("Failed to query device surface support");
        throw std::runtime_error("Vulkan did an oopsie");
    }

    return surfaceSupport;
}

unsigned int VulkanDeviceSelector::CountQueueFamilyFlags(VkQueueFlags flags) const
{
    std::bitset<32> set(flags);

    return set.count();   
}

bool VulkanDeviceSelector::FlagsArePresent(VkQueueFlags flags1, VkQueueFlags flags2) const
{
    return ((flags2 & flags1) == flags1);
}

std::vector<VulkanQueueFamilyInfo> VulkanDeviceSelector::FindSuitableFamilies(std::shared_ptr<VulkanPhysicalDevice>& physicalDevice, VkQueueFlags requiredFlags)
{
    std::vector<VulkanQueueFamilyInfo> result;

    auto& queueFamilies = physicalDevice->GetQueueFamilyInfos();
    std::copy_if(queueFamilies.begin(), queueFamilies.end(), std::back_inserter(result), [&](const VulkanQueueFamilyInfo& info)
    {
        return FlagsArePresent(requiredFlags, info.Properties.queueFlags);
    });

    std::sort(result.begin(), result.end(), [&](const VulkanQueueFamilyInfo& lhs, const VulkanQueueFamilyInfo& rhs)
    {
        return CountQueueFamilyFlags(lhs.Properties.queueFlags) < CountQueueFamilyFlags(rhs.Properties.queueFlags);
    });

    return result;
}

bool VulkanDeviceSelector::DoesDeviceSupportExtensions(std::shared_ptr<VulkanPhysicalDevice>& physicalDevice)
{
    if(m_DeviceRequirements.Extensions.size() == 0)
        return true;

    Log.Info("Checking whether the device supports required extensions");
    std::set<std::string> required(m_DeviceRequirements.Extensions.begin(), m_DeviceRequirements.Extensions.end());

    Log.Info("Requested extensions");
    for(auto ext : required)
    {
        Log.Info("    ", ext);
    }
    
    Log.Info("Following extensions are available");
    for(auto extension : physicalDevice->GetExtensions())
    {
        if(required.erase(extension) > 0)
        {
            Log.Info("    ", extension);
            physicalDevice->EnableExtension(extension);
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

bool VulkanDeviceSelector::DoesDeviceSupportRequests(std::shared_ptr<VulkanPhysicalDevice>& physicalDevice)
{
    size_t requestCount = m_DeviceRequirements.Queues.size();

    // Each element represents the number of queues requested of that type which we will attempt to fulfill. ie. at the end it should all be zeros
    std::vector<int> unfulfilledRequestedQueueCount;

    for(auto& request : m_DeviceRequirements.Queues)
    {
        unfulfilledRequestedQueueCount.push_back(request.Count);
    }
    
    // Each element represents the number of queues left at that family
    std::vector<int> familyQueuesLeft;

    for(auto& family : physicalDevice->GetQueueFamilyInfos())
    {
        familyQueuesLeft.push_back(family.Properties.queueCount);
    }

    // This could be further improved by prioritizing requests based on queueFamily [flags and count] availability
    // Attempt to fullfil each queue request by assigning queues from the physical device to match the requests
    for(size_t i = 0; i < requestCount; i++)
    {
        VulkanQueueRequest& request = m_DeviceRequirements.Queues[i];

        if(request.Surface.has_value())
        {
            auto swapchainDetails = physicalDevice->GetSwapchainSupportDetails(request.Surface.value());

            if(swapchainDetails.Formats.empty())
            {
                Log.Info("Physical device provided 0 swapchain formats");
                // No swapchain formats available
                return false;
            }

            if(swapchainDetails.PresentModes.empty())
            {
                Log.Info("Physical device provided 0 swapchain present modes");
                // No present modes available
                return false;
            }
        }

        auto suitableFamilies = FindSuitableFamilies(physicalDevice, request.Flags);

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

std::string VulkanDeviceSelector::StandardFlagsToString(VkQueueFlags flags) const
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

VkQueueFlags VulkanDeviceSelector::SanitizeQueueFlags(VkQueueFlags flags) const
{
    const VkQueueFlags validFlagMask = VK_QUEUE_GRAPHICS_BIT |
        VK_QUEUE_COMPUTE_BIT |
        VK_QUEUE_TRANSFER_BIT |
        VK_QUEUE_SPARSE_BINDING_BIT |
        VK_QUEUE_PROTECTED_BIT;

    return (flags & validFlagMask);
}

int VulkanDeviceSelector::ScoreDevice(std::shared_ptr<VulkanPhysicalDevice>& physicalDevice)
{
    int score = 0;

    if(physicalDevice->GetProperties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;
    
    // Do check for extension support before checking the requests
    // Current  reason being the swapchain. Check the extension first, capabilities second
    bool extensionSupport = DoesDeviceSupportExtensions(physicalDevice);

    if(!extensionSupport)
    {
        return -1;
    }
    
    bool queueRequestsSupport = DoesDeviceSupportRequests(physicalDevice);
    
    if(!queueRequestsSupport)
    {
        return -1;
    }

    return score;
}

