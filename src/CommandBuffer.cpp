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
	vkFreeCommandBuffers(m_logicalDevice->getHandle(), m_commandPool->getHandle(), 1, &m_commandBuffer);
}

void vdu::CommandBuffer::reset()
{
	VDU_VK_CHECK_RESULT(vkResetCommandBuffer(m_commandBuffer, 0));
}

void vdu::CommandBuffer::begin(VkCommandBufferUsageFlagBits usage)
{
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = usage;
	vkBeginCommandBuffer(m_commandBuffer, &beginInfo);
}

void vdu::CommandBuffer::end()
{
	vkEndCommandBuffer(m_commandBuffer);
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
	vkFreeCommandBuffers(m_logicalDevice->getHandle(), m_commandPool->getHandle(), m_count, m_commandBuffers);
}
