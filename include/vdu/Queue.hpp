#pragma once
#include "LogicalDevice.hpp"
#include "CommandBuffer.hpp"
#include "Synchro.hpp"

namespace vdu
{
	struct QueueSubmission
	{
	public:

		void addWait(VkSemaphore wait, VkPipelineStageFlags stage);
		void addCommands(vdu::CommandBuffer* cmd);
		void addSignal(VkSemaphore signal);

		const std::vector<VkSemaphore>& getWaitSemaphores() const { return m_waitSemaphores; }
		const std::vector<VkSemaphore>& getSignalSemaphores() const { return m_signalSemaphores; }
		const std::vector<VkPipelineStageFlags>& getWaitStages() const { return m_waitStages; }
		const std::vector<VkCommandBuffer>& getCommandBuffers() const { return m_commandBuffers; }

		void clear();

	private:

		std::vector<VkSemaphore> m_waitSemaphores;
		std::vector<VkPipelineStageFlags> m_waitStages;
		std::vector<VkCommandBuffer> m_commandBuffers;
		std::vector<VkSemaphore> m_signalSemaphores;

	};

	/*
	Wrapper for logical queue
	*/
	class Queue
	{
		friend class LogicalDevice;
	public:
		Queue();

		void prepare(uint32_t queueFamilyIndex, float priority);

		void submit(VkSubmitInfo* info, uint32_t count = 1, VkFence fence = VK_NULL_HANDLE);
		void submit(const QueueSubmission& qSubmit, const vdu::Fence& fence = vdu::Fence());
		void submit(const std::vector<QueueSubmission>& qSubmits, const vdu::Fence& fence = vdu::Fence());
		void present(const VkPresentInfoKHR* info);
		void waitIdle();

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
}