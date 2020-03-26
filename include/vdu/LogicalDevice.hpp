#pragma once
#include "PCH.hpp"

namespace vdu
{
class Queue;
class PhysicalDevice;

/*
	Wrapper for logical device
	*/
class LogicalDevice
{
public:
	VkResult create(PhysicalDevice *physicalDevice);

	void destroy();

	VkDevice getHandle() { return m_device; }
	const PhysicalDevice *getPhysicalDevice() const { return m_physicalDevice; }

	void addQueue(Queue *queue);
	void addExtension(const char *extensionName);
	void addLayer(const char *layerName);
	void setEnabledDeviceFeatures(const VkPhysicalDeviceFeatures &pdf);

	typedef void (*PFN_vkErrorCallback)(VkResult error, const std::string &message);

	enum VduDebugLevel
	{
		Info,
		Warning,
		Error
	};

	typedef void (*PFN_vduDebugCallback)(VduDebugLevel level, const std::string &message);

	void setVkErrorCallback(PFN_vkErrorCallback errorCallback);
	void setVduDebugCallback(PFN_vduDebugCallback errorCallback);

	void _internalReportVkError(VkResult error, const std::string &message);
	void _internalReportVduDebug(VduDebugLevel level, const std::string &message);

private:
	VkDevice m_device = 0;

	std::set<Queue *> m_queues;
	std::map<uint32_t, std::vector<float>> m_queueFamilyCountsPriorities; // and priorities

	VkPhysicalDeviceFeatures m_enabledDeviceFeatures = {};

	PFN_vkErrorCallback m_vkErrorCallbackFunc = nullptr;
	PFN_vduDebugCallback m_vduDebugCallbackFunc = nullptr;

	/*
			Enabled layers and extensions
		*/
	std::vector<const char *> m_enabledExtensions;
	std::vector<const char *> m_enabledLayers;

	PhysicalDevice *m_physicalDevice = nullptr;
};
} // namespace vdu