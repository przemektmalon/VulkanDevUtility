#include "CommandBuffer.hpp"

void vdu::CommandBuffer::setLevel(VkCommandBufferLevel level)
{
	m_level = level;
}

void vdu::CommandBuffer::allocate(LogicalDevice * logicalDevice, CommandPool * commandPool)
{
	m_commandPool = commandPool;
	m_logicalDevice = logicalDevice;

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool->getHandle();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VDU_VK_CHECK_RESULT(vkAllocateCommandBuffers(m_logicalDevice->getHandle(), &allocInfo, &m_commandBuffer));
}

void vdu::CommandBuffer::free()
{
	VDU_VK_VALIDATE(vkFreeCommandBuffers(m_logicalDevice->getHandle(), m_commandPool->getHandle(), 1, &m_commandBuffer));
}

void vdu::CommandBufferArray::setLevel(VkCommandBufferLevel level)
{
	m_level = level;
}

void vdu::CommandBufferArray::allocate(LogicalDevice * logicalDevice, CommandPool * commandPool, uint32_t count)
{
	m_commandPool = commandPool;
	m_logicalDevice = logicalDevice;
	m_count = count;

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool->getHandle();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = m_count;

	m_commandBuffers = new VkCommandBuffer[m_count];

	VDU_VK_CHECK_RESULT(vkAllocateCommandBuffers(m_logicalDevice->getHandle(), &allocInfo, m_commandBuffers));
}

void vdu::CommandBufferArray::free()
{
	VDU_VK_VALIDATE(vkFreeCommandBuffers(m_logicalDevice->getHandle(), m_commandPool->getHandle(), m_count, m_commandBuffers));
}
