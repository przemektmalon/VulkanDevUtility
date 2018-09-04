#pragma once
#include "LogicalDevice.hpp"
#include "MemoryPools.hpp"

namespace vdu
{
	class CommandBuffer
	{
	public:
		CommandBuffer() : m_logicalDevice(0), m_commandPool(0), m_commandBuffer(0), m_level(VK_COMMAND_BUFFER_LEVEL_PRIMARY) {}

		void setLevel(VkCommandBufferLevel level);
		void allocate(LogicalDevice* logicalDevice, CommandPool* commandPool);
		void free();

		void begin(VkCommandBufferUsageFlagBits usage = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
		void end();

		const VkCommandBuffer& getHandle() const { return m_commandBuffer; }

	private:

		LogicalDevice * m_logicalDevice;
		CommandPool * m_commandPool;

		VkCommandBuffer m_commandBuffer;
		VkCommandBufferLevel m_level;
	};

	class CommandBufferArray
	{
	public:
		CommandBufferArray() : m_logicalDevice(0), m_commandPool(0), m_commandBuffers(0), m_level(VK_COMMAND_BUFFER_LEVEL_PRIMARY) {}

		void setLevel(VkCommandBufferLevel level);
		void allocate(LogicalDevice* logicalDevice, CommandPool* commandPool, uint32_t count);
		void free();

		uint32_t size() { return m_count; }

		const VkCommandBuffer& getHandle(uint32_t index) { return m_commandBuffers[index]; }

	private:

		LogicalDevice * m_logicalDevice;
		CommandPool * m_commandPool;

		VkCommandBuffer * m_commandBuffers;
		VkCommandBufferLevel m_level;
		uint32_t m_count;
	};
}