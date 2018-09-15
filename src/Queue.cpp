#include "PCH.hpp"
#include "Queue.hpp"

vdu::Queue::Queue() : m_queue(0), m_queueFamilyIndex(~(uint32_t(0))), m_queueIndex(~(uint32_t(0))), m_priority(1.f)
{
}

void vdu::Queue::prepare(uint32_t queueFamilyIndex, float priority)
{
	m_queueFamilyIndex = queueFamilyIndex;
	m_priority = priority;
}

void vdu::Queue::submit(VkSubmitInfo * info, uint32_t count, VkFence fence)
{
	VDU_VK_CHECK_RESULT(vkQueueSubmit(m_queue, count, info, fence));
}

void vdu::Queue::submit(const QueueSubmission& qSubmit, const vdu::Fence& fence)
{
	auto& waitsems = qSubmit.getWaitSemaphores();
	auto& cmds = qSubmit.getCommandBuffers();
	auto& sigs = qSubmit.getSignalSemaphores();
	auto& waitstages = qSubmit.getWaitStages();

	VkSubmitInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.waitSemaphoreCount = waitsems.size();
	info.pWaitSemaphores = waitsems.data();
	info.pWaitDstStageMask = waitstages.data();
	info.commandBufferCount = cmds.size();
	info.pCommandBuffers = cmds.data();
	info.signalSemaphoreCount = sigs.size();
	info.pSignalSemaphores = sigs.data();

	VDU_VK_CHECK_RESULT(vkQueueSubmit(m_queue, 1, &info, fence.getHandle()));
}

void vdu::Queue::submit(const std::vector<QueueSubmission>& qSubmits, const vdu::Fence& fence)
{
	auto submitInfos = new VkSubmitInfo[qSubmits.size()];
	uint32_t i = 0;
	for (auto& submission : qSubmits)
	{
		auto& waitsems = submission.getWaitSemaphores();
		auto& cmds = submission.getCommandBuffers();
		auto& sigs = submission.getSignalSemaphores();
		auto& waitstages = submission.getWaitStages();

		submitInfos[i] = {};
		submitInfos[i].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfos[i].waitSemaphoreCount = waitsems.size();
		submitInfos[i].pWaitSemaphores = waitsems.data();
		submitInfos[i].pWaitDstStageMask = waitstages.data();
		submitInfos[i].commandBufferCount = cmds.size();
		submitInfos[i].pCommandBuffers = cmds.data();
		submitInfos[i].signalSemaphoreCount = sigs.size();
		submitInfos[i].pSignalSemaphores = sigs.data();

		++i;
	}

	VDU_VK_CHECK_RESULT(vkQueueSubmit(m_queue, i, submitInfos, fence.getHandle()));
}

void vdu::Queue::present(const VkPresentInfoKHR * info)
{
	VDU_VK_CHECK_RESULT(vkQueuePresentKHR(m_queue, info));
}

void vdu::Queue::present(const QueuePresentation & qPresent)
{
	auto& waitsems = qPresent.getWaitSemaphores();
	auto& swaps = qPresent.getSwapchains();
	auto& indices = qPresent.getImageIndices();
	auto& displayInfo = qPresent.getDisplayPresentInfo();

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	if (displayInfo.sType != 0)
		presentInfo.pNext = &displayInfo;
	presentInfo.waitSemaphoreCount = waitsems.size();
	presentInfo.pWaitSemaphores = waitsems.data();
	presentInfo.swapchainCount = swaps.size();
	presentInfo.pSwapchains = swaps.data();
	presentInfo.pImageIndices = indices.data();

	VDU_VK_CHECK_RESULT(vkQueuePresentKHR(m_queue, &presentInfo));
}

void vdu::Queue::waitIdle()
{
	VDU_VK_CHECK_RESULT(vkQueueWaitIdle(m_queue));
}

void vdu::QueueSubmission::addWait(VkSemaphore wait, VkPipelineStageFlags stage)
{
	m_waitSemaphores.push_back(wait);
	m_waitStages.push_back(stage);
}

void vdu::QueueSubmission::addCommands(vdu::CommandBuffer * cmd)
{
	m_commandBuffers.push_back(cmd->getHandle());
}

void vdu::QueueSubmission::addCommands(VkCommandBuffer cmd)
{
	m_commandBuffers.push_back(cmd);
}

void vdu::QueueSubmission::addSignal(VkSemaphore signal)
{
	m_signalSemaphores.push_back(signal);
}

void vdu::QueueSubmission::clear()
{
	m_waitSemaphores.clear();
	m_waitStages.clear();
	m_commandBuffers.clear();
	m_signalSemaphores.clear();
}

void vdu::QueuePresentation::addWait(VkSemaphore wait)
{
	m_waitSemaphores.push_back(wait);
}

void vdu::QueuePresentation::addSwapchain(const vdu::Swapchain& swapchain, uint32_t imageIndex, const VkRect2D & srcRect, const VkRect2D & dstRect, VkBool32 persistant)
{
	m_swapchains.push_back(swapchain.getHandle());
	m_imageIndices.push_back(imageIndex);
	m_displayInfo = {};

	if (srcRect.offset.x != std::numeric_limits<int32_t>::lowest() && srcRect.offset.y != std::numeric_limits<int32_t>::lowest())
	{
		m_displayInfo.sType = VK_STRUCTURE_TYPE_DISPLAY_PRESENT_INFO_KHR;
		m_displayInfo.srcRect = srcRect;
		m_displayInfo.dstRect = dstRect;
		m_displayInfo.persistent = persistant;
	}
}
