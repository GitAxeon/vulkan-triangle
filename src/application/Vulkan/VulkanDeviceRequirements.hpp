#pragma once

#include <Vulkan/vulkan.hpp>

#include <map>
#include <optional>
#include <set>
#include <vector>

/*
    VulkanQueueRequest uses these structs to request VkQueue handles
*/
struct QueueLocation
{
    // Index of a queue family
    uint32_t FamilyIndex;

    // Queue index within the family
    uint32_t Index;
};

/*
   User filled struct that represents the capabilities required by the device, number of queues and priorities
   Multiple ones can be filled which is why they need to be processed before creating queues infos
*/
struct VulkanQueueRequest
{
    VkQueueFlags Flags;
    std::optional<VkSurfaceKHR> Surface;
    uint32_t Count;
    std::vector<float> Priorities;

    std::set<uint32_t> GetFamilyIndices();
    
private:
    friend class VulkanDeviceRequirements;
    friend class VulkanDeviceSelector;
    friend class VulkanDevice;

    // Gets filled with locations to requested queues within the physical device
    std::vector<QueueLocation> QueueLocations;
};

/*
    Further down the pipeline the VulkanQueueRequest get sorted into QueueFamilyCreateInfo
    "Private"
*/ 
struct QueueFamilyCreateInfo
{
    QueueFamilyCreateInfo() : QueueCount(0), QueuePriorities() {}

    int QueueCount;
    std::vector<float> QueuePriorities;
};

/*
    User filled object representing the types of queues needed and extensions that the application uses
*/
class VulkanDeviceRequirements
{
public:
    VulkanDeviceRequirements() : Queues({}), Extensions({}) { };

public:
    std::vector<VulkanQueueRequest> Queues;
    std::vector<std::string> Extensions;

private:
    friend class VulkanDeviceSelector;
    friend class VulkanDevice;

    void FillQueuePrioritiesIfNeeded();
    std::map<uint32_t, QueueFamilyCreateInfo> CombineQueueRequestIntoQueueFamilyCreateInfos();
};