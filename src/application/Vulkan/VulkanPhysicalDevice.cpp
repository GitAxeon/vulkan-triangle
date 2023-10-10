#include "VulkanPhysicalDevice.hpp"
#include "VulkanInstance.hpp"

VulkanPhysicalDevice::VulkanPhysicalDevice(std::shared_ptr<VulkanInstance> vulkanInstance, VkPhysicalDevice deviceHandle, uint32_t deviceId)
    : m_Instance(vulkanInstance), m_PhysicalDevice(deviceHandle), m_PhysicalDeviceIndex(deviceId)
{
    QueryDeviceProperties();
    QueryDeviceFeatures();
    QueryDeviceExtensionProperties();
    
    // m_Extension contains only the names of available extensions
    for(auto& extension : m_ExtensionProperties)
    {
        m_Extensions.push_back(extension.extensionName);
    }

    QueryDeviceQueueFamilyInfos();
    
    Log.Info("Created VulkanPhysicalDevice");
}

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
    Log.Info("Destructed VulkanPhysicalDevice");
}

std::vector<VkQueueFamilyProperties> VulkanPhysicalDevice::EnumerateDeviceQueueFamilyProperties()
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilies.data());
    
    return queueFamilies;
}

void VulkanPhysicalDevice::QueryDeviceProperties()
{
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);
};


void VulkanPhysicalDevice::QueryDeviceFeatures()
{
    vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);
}

void VulkanPhysicalDevice::QueryDeviceExtensionProperties()
{
    uint32_t extensionCount;
    VkResult operationResult;

    operationResult = vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);

    if(operationResult != VK_SUCCESS)
    {
        Log.Error("vkEnumerateDeviceExtensionProperties #1 call failed");
        throw std::runtime_error("Failed to query physical device extension properties");
    }

    // Resize the vector to accommodate the extensions
    m_ExtensionProperties.resize(extensionCount);

    operationResult = vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, m_ExtensionProperties.data());

    if(operationResult != VK_SUCCESS)
    {
        Log.Error("vkEnumerateDeviceExtensionProperties #2 call failed");
        throw std::runtime_error("Failed to query physical device extension properties");
    }
}

void VulkanPhysicalDevice::QueryDeviceQueueFamilyInfos()
{
    uint32_t familyIndex = 0;
    for(auto familyProperties : EnumerateDeviceQueueFamilyProperties())
    {
        VulkanQueueFamilyInfo familyInfo;
        familyInfo.Properties = familyProperties;
        familyInfo.Index = familyIndex;
        familyInfo.PresentSupport = false;

        m_QueueFamilies.push_back(familyInfo);

        familyIndex++;
    }
}

const SwapchainSupportDetails VulkanPhysicalDevice::GetSwapchainSupportDetails(VkSurfaceKHR surface)
{
    SwapchainSupportDetails swapchainSupportDetails;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, surface, &swapchainSupportDetails.Capabilities);

    uint32_t formatCount;
    VkResult operationResult;
    operationResult = vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, surface, &formatCount, nullptr);

    if(operationResult != VK_SUCCESS)
    {
        Log.Error("VkGetPhysicalDeviceSurfaceFormatsKHR #1 failed");
        throw std::runtime_error("Failed to query device surface formats count");
    }

    if(formatCount > 0)
    {
        swapchainSupportDetails.Formats.resize(formatCount);
        operationResult = vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, surface, &formatCount, swapchainSupportDetails.Formats.data());

        if(operationResult != VK_SUCCESS)
        {
            Log.Error("VkGetPhysicalDeviceSurfaceFormatsKHR #2 failed");
            throw std::runtime_error("Failed to query device surface formats");
        }
    }

    uint32_t presentModeCount;
    operationResult = vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, surface, &presentModeCount, nullptr);

    if(operationResult != VK_SUCCESS)
    {
        Log.Error("VkGetPhysicalDeviceSurfacePresentModeKHR #1 failed");
        throw std::runtime_error("Failed to query device present mode count");
    }

    if(presentModeCount > 0)
    {
        swapchainSupportDetails.PresentModes.resize(presentModeCount);
        operationResult = vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, surface, &presentModeCount, swapchainSupportDetails.PresentModes.data());

        if(operationResult != VK_SUCCESS)
        {
            Log.Error("VkGetPhysicalDeviceSurfacePresentModesKHR #2 failed");
            throw std::runtime_error("Failed to query device present modes");
        }
    }

    return swapchainSupportDetails;
}

std::vector<VkPhysicalDevice> VulkanPhysicalDevice::EnumeratePhysicalDevices(std::shared_ptr<VulkanInstance> vulkanInstance)
{
    uint32_t deviceCount = 0;
    VkResult operationResult;
    
    operationResult = vkEnumeratePhysicalDevices(vulkanInstance->GetInstance(), &deviceCount, nullptr);

    if(operationResult != VK_SUCCESS)
    {
        Log.Error("vkEnumeratePhysicalDevices failed at getting deviceCount VkResult", operationResult);
    }

    if(deviceCount == 0)
    {
        Log.Info("Failed to find GPUs with Vulkan support!");
        throw std::runtime_error("No suitable GraphicsDevice found");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    operationResult = vkEnumeratePhysicalDevices(vulkanInstance->GetInstance(), &deviceCount, devices.data());

    if(operationResult != VK_SUCCESS)
    {
        Log.Error("vkEnumeratePhysicalDevices failed listing devices VkResult", operationResult);
    }

    return devices;
}

VkPhysicalDevice VulkanPhysicalDevice::GetDevice(std::shared_ptr<VulkanInstance> instance, int id)
{
    auto devices = EnumeratePhysicalDevices(instance);
    
    assert((id > -1) && (id < devices.size()));

    return devices[id];
}

void VulkanPhysicalDevice::EnableExtension(const std::string& extension)
{
    m_EnabledExtensions.push_back(extension);
}