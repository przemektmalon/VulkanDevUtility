#pragma once
#include "PCH.hpp"
#include "PhysicalDevice.hpp"

namespace vdu
{
	/*
	Wrapper for logical queue
	*/
	class Queue
	{
		friend class LogicalDevice;
	public:
		Queue();

		void prepare(uint32_t queueFamilyIndex, float priority);

		VkQueue getHandle() { return m_queue; }
		uint32_t getFamilyIndex() { return m_queueFamilyIndex; }
		uint32_t getIndex() { return m_queueIndex; }
		float getPriority() { return m_priority; }

	private:

		/*
		This is set when the queue is added to a logical device
		*/
		void setIndex(uint32_t index) { m_queueIndex = index; }

		/*
		This is set when the logical device that owns the queue is created
		*/
		void setQueueHandle(VkQueue handle) { m_queue = handle; }

		/*
		Queues handle
		*/
		VkQueue m_queue;

		/*
		Queue's family index
		*/
		uint32_t m_queueFamilyIndex;

		/*
		Queue index within family
		*/
		uint32_t m_queueIndex;

		/*
		Queue priority
		*/
		float m_priority;


	};

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