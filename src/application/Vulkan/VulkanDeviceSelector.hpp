#pragma once

#include "VulkanPhysicalDevice.hpp"
#include "VulkanDeviceRequirements.hpp"
#include "VulkanDevice.hpp"

#include <Vulkan/vulkan.hpp>

#include <algorithm>
#include <bitset>
#include <map>
#include <set>

class VulkanDeviceSelector
{
public:
    VulkanDeviceSelector() = delete;
    VulkanDeviceSelector(const VulkanDeviceSelector&) = delete;
    
    VulkanDeviceSelector(std::shared_ptr<VulkanInstance> vulkanInstance, std::shared_ptr<VulkanDeviceRequirements> requirements);

    std::shared_ptr<VulkanDevice> GetDevice();

    std::shared_ptr<VulkanInstance> GetInstance() const { return m_Instance; }

private:

    static bool CheckSurfaceSupport(std::shared_ptr<VulkanPhysicalDevice> physicalDevice, uint32_t familyIndex, VkSurfaceKHR surface);

    // Return the number of enabled flags
    unsigned int CountQueueFamilyFlags(VkQueueFlags flags) const;

    // return true if flags1 are present in flags2
    bool FlagsArePresent(VkQueueFlags flags1, VkQueueFlags flags2) const;

    // Return all families containing atleast |request.Flags| flags (ascending order)
    std::vector<VulkanQueueFamilyInfo> FindSuitableFamilies(std::shared_ptr<VulkanPhysicalDevice>& physicalDevice, VkQueueFlags requiredFlags);

    bool DoesDeviceSupportExtensions(std::shared_ptr<VulkanPhysicalDevice>& physicalDevice);

    // Returns true if the device can provide all requested queues
    bool DoesDeviceSupportRequests(std::shared_ptr<VulkanPhysicalDevice>& physicalDevice);

    std::string StandardFlagsToString(VkQueueFlags flags) const;

    VkQueueFlags SanitizeQueueFlags(VkQueueFlags flags) const;

    int ScoreDevice(std::shared_ptr<VulkanPhysicalDevice>& device);

private:
    std::shared_ptr<VulkanInstance> m_Instance;
    std::shared_ptr<VulkanDeviceRequirements> m_DeviceRequirements;

    std::shared_ptr<VulkanDevice> m_Device;
};