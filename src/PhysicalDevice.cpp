#include "PCH.hpp"
#include "PhysicalDevice.hpp"
#include "Initializers.hpp"

#define VK_CHECK_RESULT(f)        \
	{                             \
		auto result = f;          \
		if (result != VK_SUCCESS) \
			return result;        \
	}

VkPhysicalDevice vdu::PhysicalDevice::getHandle() const
{
	return m_physicalDevice;
}

std::vector<vdu::QueueFamily> &vdu::PhysicalDevice::getQueueFamilies()
{
	return m_queueFamilies;
}

uint32_t vdu::PhysicalDevice::findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
	for (uint32_t i = 0; i < m_memoryProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (m_memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
	return ~(uint32_t(0));
}

VkFormat vdu::PhysicalDevice::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	return VK_FORMAT_UNDEFINED;
}

VkFormat vdu::PhysicalDevice::findOptimalDepthFormat() const
{
	return findSupportedFormat(
		{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

const VkSurfaceCapabilitiesKHR &vdu::PhysicalDevice::getSurfaceCapabilities() const
{
	return m_surfaceCapabilities;
}

const std::vector<VkSurfaceFormatKHR> &vdu::PhysicalDevice::getSurfaceFormats() const
{
	return m_surfaceFormats;
}

const std::vector<VkPresentModeKHR> &vdu::PhysicalDevice::getPresentModes() const
{
	return m_presentModes;
}

VkPhysicalDeviceProperties vdu::PhysicalDevice::getDeviceProperties() const
{
	return m_deviceProperties;
}

void vdu::PhysicalDevice::queryDetails()
{
	// Query device properties and features
	{
		vkGetPhysicalDeviceProperties(m_physicalDevice, &m_deviceProperties);
		vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_deviceFeatures);

		switch (m_deviceProperties.deviceType)
		{
		case (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU):
			m_suitabilityScore += 2000;
			break;
		case (VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU):
			m_suitabilityScore += 500;
			break;
		case (VK_PHYSICAL_DEVICE_TYPE_CPU):
			m_suitabilityScore += 100;
			break;
		default:
			m_suitabilityScore -= 10000;
			break;
		}
	}

	std::vector<VkQueueFamilyProperties> queueFamilyProperties;

	// Query queue family properties
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);
		queueFamilyProperties.resize(queueFamilyCount);
		m_queueFamilies.reserve(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
	}

	// Choose queue families
	{
		int i = 0;
		for (const auto &queueFamily : queueFamilyProperties)
		{
			m_queueFamilies.push_back(QueueFamily(this, queueFamily, i));
			++i;
		}
	}

	// Query device memory properties
	{
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_memoryProperties);
	}
}

VkResult vdu::PhysicalDevice::querySurfaceCapabilities(VkSurfaceKHR surface)
{
	// Query surface capabilities
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, surface, &m_surfaceCapabilities));

	// Query surface formats
	{
		uint32_t formatCount;
		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &formatCount, nullptr));

		if (formatCount != 0)
		{
			m_surfaceFormats.resize(formatCount);
			VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &formatCount, m_surfaceFormats.data()));
		}
		else
			m_suitabilityScore -= 100000;
	}

	// Query surface present modes
	{
		uint32_t presentModeCount;
		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &presentModeCount, nullptr));

		if (presentModeCount != 0)
		{
			m_presentModes.resize(presentModeCount);
			VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &presentModeCount, m_presentModes.data()));
		}
		else
			m_suitabilityScore -= 100000;
	}

	for (auto &qf : m_queueFamilies)
	{
		auto result = qf.queryPresentCapability(surface);
		if (result != VK_SUCCESS)
			return result;
	}
	return VK_SUCCESS;
}
