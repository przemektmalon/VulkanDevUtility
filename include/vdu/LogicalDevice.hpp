#pragma once
#include "PCH.hpp"
#include "PhysicalDevice.hpp"

namespace vdu
{
	class Queue;

	/*
	Wrapper for logical device
	*/
	class LogicalDevice
	{
	public:
		void create(PhysicalDevice* physicalDevice);

		void destroy();

		VkDevice getHandle() { return m_device; }
		const PhysicalDevice* getPhysicalDevice() const { return m_physicalDevice; }

		void addQueue(Queue* queue);
		void addExtension(const char * extensionName);
		void addLayer(const char * layerName);
		void setEnabledDeviceFeatures(const VkPhysicalDeviceFeatures& pdf);
		
	private:

		VkDevice m_device;

		std::set<Queue*> m_queues;
		std::map<uint32_t, std::vector<float>> m_queueFamilyCountsPriorities; // and priorities

		VkPhysicalDeviceFeatures m_enabledDeviceFeatures;

		/*
			Enabled layers and extensions
		*/
		std::vector<const char*> m_enabledExtensions;
		std::vector<const char*> m_enabledLayers;

		PhysicalDevice* m_physicalDevice;
	};
}