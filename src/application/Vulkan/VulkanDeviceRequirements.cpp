#include "VulkanDeviceRequirements.hpp"

void VulkanDeviceRequirements::FillQueuePrioritiesIfNeeded()
{
    // Maybe the device Requirements can do this part? 
    for(auto& request : Queues)
        if(request.Priorities.size() < request.Count)
            request.Priorities.resize(request.Count, 1.0f);
}

std::map<uint32_t, QueueFamilyCreateInfo> VulkanDeviceRequirements::CombineQueueRequestIntoQueueFamilyCreateInfos()
{
    FillQueuePrioritiesIfNeeded();
    
    std::map<uint32_t, QueueFamilyCreateInfo> result;
    
    for(auto& request : Queues)
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

            QueueFamilyCreateInfo& queueFamilyCreateInfo = result[familyIndex];
            queueFamilyCreateInfo.QueueCount++;

            uint32_t queueIndex = queueLocation.Index;

            if (queueFamilyCreateInfo.QueuePriorities.size() < queueIndex + 1)
            {
                queueFamilyCreateInfo.QueuePriorities.resize(queueIndex + 1);
            }

            queueFamilyCreateInfo.QueuePriorities[queueIndex] = request.Priorities[priorityIndex];
            
            priorityIndex++;
        }
    }

    return result;
}