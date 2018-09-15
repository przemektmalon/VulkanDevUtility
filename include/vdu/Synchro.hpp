#pragma once
#include "PCH.hpp"

namespace vdu
{
	class LogicalDevice;
	class CommandBuffer;

	class Event
	{
	public:
		void create(LogicalDevice* device);
		void destroy();

		VkEvent getHandle() { return m_event; }

		VkResult getStatus();
		bool isSet();

		void cmdSet(CommandBuffer* cmd, VkPipelineStageFlagBits stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
		void cmdReset(CommandBuffer* cmd, VkPipelineStageFlagBits stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
		void cmdSet(VkCommandBuffer& cmd, VkPipelineStageFlagBits stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
		void cmdReset(VkCommandBuffer& cmd, VkPipelineStageFlagBits stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
		void set();
		void reset();

	private:

		LogicalDevice * m_logicalDevice = nullptr;
		VkEvent m_event = 0;
	};

	class Fence
	{
	public:
		void create(LogicalDevice* device, bool initiallySignalled = false);
		void destroy();

		VkFence getHandle() const { return m_fence; }

		VkResult getStatus() const;
		bool isSignalled() const;

		void reset();
		void wait() const;

	private:

		LogicalDevice * m_logicalDevice = nullptr;
		VkFence m_fence = 0;
	};
}