#pragma once
#include "PCH.hpp"
#include "QueueFamily.hpp"

namespace vdu
{

/*
    Wrapper for a physical device (GPU/CPU/Integrated)
*/
class PhysicalDevice
{
public:
    PhysicalDevice(VkPhysicalDevice device) : m_physicalDevice(device)
    {
        queryDetails();
    }

    /*
        Query surface capabilities
    */
    VkResult querySurfaceCapabilities(VkSurfaceKHR surface);

    VkPhysicalDevice getHandle() const;

    std::vector<QueueFamily> &getQueueFamilies();

    /*
        Functions for finding memory type indices, supported formats from a list of
        candidates, and an optimal depth format
    */
    uint32_t findMemoryTypeIndex(uint32_t typeFilter,
                                 VkMemoryPropertyFlags properties) const;
    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                                 VkImageTiling tiling,
                                 VkFormatFeatureFlags features) const;
    VkFormat findOptimalDepthFormat() const;

    const VkSurfaceCapabilitiesKHR &getSurfaceCapabilities() const;
    const std::vector<VkSurfaceFormatKHR> &getSurfaceFormats() const;
    const std::vector<VkPresentModeKHR> &getPresentModes() const;

    VkPhysicalDeviceProperties getDeviceProperties() const;

private:
    /*
        Query and fill in properties and features
    */
    void queryDetails();

    /*
        Dedicated GPUs are better than CPUs
    */
    int32_t m_suitabilityScore;

    std::vector<QueueFamily> m_queueFamilies;

    VkPhysicalDevice m_physicalDevice;
    VkPhysicalDeviceProperties m_deviceProperties;
    VkPhysicalDeviceFeatures m_deviceFeatures;

    VkPhysicalDeviceMemoryProperties m_memoryProperties;

    VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
    std::vector<VkPresentModeKHR> m_presentModes;
};
} // namespace vdu