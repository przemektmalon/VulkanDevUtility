#pragma once
#include "PCH.hpp"
#include "LogicalDevice.hpp"

/*
Wrappers for command, descriptor, and query pools
*/

namespace vdu
{
	class DescriptorPool
	{
	public:
		DescriptorPool();

		void create(LogicalDevice* logicalDevice);

		void destroy();

		VkDescriptorPool getHandle() { return m_descriptorPool; }

		void addPoolCount(VkDescriptorType type, uint32_t count);
		void addSetCount(uint32_t count);

		void setFreeable(bool freeable) { m_freeable = freeable; }

	private:

		VkDescriptorPool m_descriptorPool;
		bool m_freeable;

		std::map<VkDescriptorType, uint32_t> m_descriptorTypeCounts;
		uint32_t m_maxSets;

		LogicalDevice* m_logicalDevice;
	};
}