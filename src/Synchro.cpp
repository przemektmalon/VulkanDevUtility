#include "PCH.hpp"
#include "Synchro.hpp"
#include "LogicalDevice.hpp"
#include "CommandBuffer.hpp"

void vdu::Event::create(LogicalDevice * device)
{
	m_logicalDevice = device;
	VkEventCreateInfo eci = { VK_STRUCTURE_TYPE_EVENT_CREATE_INFO, nullptr, 0 };
	VDU_VK_CHECK_RESULT(vkCreateEvent(m_logicalDevice->getHandle(), &eci, nullptr, &m_event), "creating event");
}

void vdu::Event::destroy()
{
	vkDestroyEvent(m_logicalDevice->getHandle(), m_event, nullptr);
}

VkResult vdu::Event::getStatus()
{
	return vkGetEventStatus(m_logicalDevice->getHandle(), m_event);
}

bool vdu::Event::isSet()
{
	return vkGetEventStatus(m_logicalDevice->getHandle(), m_event) == VK_EVENT_SET;
}

void vdu::Event::cmdSet(CommandBuffer * cmd, VkPipelineStageFlagBits stage)
{
	vkCmdSetEvent(cmd->getHandle(), m_event, stage);
}

void vdu::Event::cmdReset(CommandBuffer * cmd, VkPipelineStageFlagBits stage)
{
	vkCmdResetEvent(cmd->getHandle(), m_event, stage);
}

void vdu::Event::cmdSet(VkCommandBuffer & cmd, VkPipelineStageFlagBits stage)
{
	vkCmdSetEvent(cmd, m_event, stage);
}

void vdu::Event::cmdReset(VkCommandBuffer & cmd, VkPipelineStageFlagBits stage)
{
	vkCmdResetEvent(cmd, m_event, stage);
}

void vdu::Event::set()
{
	VDU_VK_CHECK_RESULT(vkSetEvent(m_logicalDevice->getHandle(), m_event), "setting event");
}

void vdu::Event::reset()
{
	VDU_VK_CHECK_RESULT(vkResetEvent(m_logicalDevice->getHandle(), m_event), "resetting event");
}

vdu::Fence::Fence(LogicalDevice * device, bool initiallySignalled)
{
	create(device, initiallySignalled);
}

void vdu::Fence::create(LogicalDevice * device, bool initiallySignalled)
{
	m_logicalDevice = device;
	VkFenceCreateInfo fci = {};
	fci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fci.flags = initiallySignalled ? 1 : 0;
	VDU_VK_CHECK_RESULT(vkCreateFence(m_logicalDevice->getHandle(), &fci, nullptr, &m_fence), "creating fence");
}

void vdu::Fence::destroy()
{
	vkDestroyFence(m_logicalDevice->getHandle(), m_fence, nullptr);
}

VkResult vdu::Fence::getStatus() const
{
	return vkGetFenceStatus(m_logicalDevice->getHandle(), m_fence);
}

bool vdu::Fence::isSignalled() const
{
	return vkGetFenceStatus(m_logicalDevice->getHandle(), m_fence) == VK_SUCCESS;
}

void vdu::Fence::reset()
{
	VDU_VK_CHECK_RESULT(vkResetFences(m_logicalDevice->getHandle(), 1, &m_fence), "resetting fence");
}

VkResult vdu::Fence::wait(uint64_t timeout) const
{
	auto result = vkWaitForFences(m_logicalDevice->getHandle(), 1, &m_fence, true, timeout);
	if (result == VK_SUCCESS || result == VK_TIMEOUT)
		return result;
	else {
		m_logicalDevice->_internalReportVkError(result, "Encountered Vulkan error while waiting for fence");
	}
}
