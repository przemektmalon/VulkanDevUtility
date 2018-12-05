#include "PCH.hpp"
#include "QueueFamily.hpp"
#include "PhysicalDevice.hpp"

vdu::QueueFamily::QueueFamily(PhysicalDevice * physicalDevice, VkQueueFamilyProperties props, uint32_t familyIndex) :
	m_physicalDevice(physicalDevice),
	m_familyProperties(props),
	m_familyIndex(familyIndex),
	m_supportsGraphics(false),
	m_supportsCompute(false),
	m_supportsTransfer(false),
	m_supportsPresent(false)
{
	queryDetails();
}

VkResult vdu::QueueFamily::queryPresentCapability(VkSurfaceKHR surface)
{
	VkBool32 surfaceSupport = false;
	auto result = vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice->getHandle(), m_familyIndex, surface, &surfaceSupport);
	if (result != VK_SUCCESS) {
		return  result;
	}
	m_supportsPresent = surfaceSupport;
	return VK_SUCCESS;
}

vdu::Queue vdu::QueueFamily::createQueue(float priority)
{
	return Queue(this, priority);
}

void vdu::QueueFamily::queryDetails()
{
	m_supportsGraphics = m_familyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
	m_supportsCompute = m_familyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT;
	m_supportsTransfer = m_familyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT;
}
