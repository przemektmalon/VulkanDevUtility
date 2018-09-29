#include "PCH.hpp"
#include "MemoryPools.hpp"
#include "Initializers.hpp"
#include "CommandBuffer.hpp"

vdu::DescriptorPool::DescriptorPool() : m_maxSets(0)
{
}

void vdu::DescriptorPool::create(LogicalDevice* logicalDevice)
{
	m_logicalDevice = logicalDevice;

	std::vector<VkDescriptorPoolSize> poolSizes;
	poolSizes.reserve(m_descriptorTypeCounts.size());

	for (auto count : m_descriptorTypeCounts)
	{
		poolSizes.push_back({ count.first, count.second });
	}

	auto dpci = vdu::initializer<VkDescriptorPoolCreateInfo>();
	dpci.flags = m_freeable ? VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : 0;
	dpci.maxSets = m_maxSets;
	dpci.poolSizeCount = poolSizes.size();
	dpci.pPoolSizes = poolSizes.data();

	VDU_VK_CHECK_RESULT(vkCreateDescriptorPool(m_logicalDevice->getHandle(), &dpci, nullptr, &m_descriptorPool));
}

void vdu::DescriptorPool::destroy()
{
	vkDestroyDescriptorPool(m_logicalDevice->getHandle(), m_descriptorPool, nullptr);
}

void vdu::DescriptorPool::addPoolCount(VkDescriptorType type, uint32_t count)
{
	m_descriptorTypeCounts[type] += count;
}

void vdu::DescriptorPool::addSetCount(uint32_t count)
{
	m_maxSets += count;
}

vdu::CommandPool::CommandPool() : m_commandPool(0), m_queueFamily(nullptr)
{
}

void vdu::CommandPool::create(LogicalDevice * logicalDevice)
{
	m_logicalDevice = logicalDevice;

	auto cpci = vdu::initializer<VkCommandPoolCreateInfo>();
	cpci.flags = m_flags;
	cpci.queueFamilyIndex = m_queueFamily->getIndex();
	
	VDU_VK_CHECK_RESULT(vkCreateCommandPool(m_logicalDevice->getHandle(), &cpci, nullptr, &m_commandPool));
}

void vdu::CommandPool::destroy()
{
	vkDestroyCommandPool(m_logicalDevice->getHandle(), m_commandPool, nullptr);
}

vdu::QueryPool::QueryPool() : m_queryPool(0), m_logicalDevice(nullptr), m_pipelineStats(0), m_queryData(nullptr)
{
}

void vdu::QueryPool::create(LogicalDevice * logicalDevice)
{
	m_logicalDevice = logicalDevice;

	VkQueryPoolCreateInfo ci = {};
	ci.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	ci.queryType = m_type;
	ci.queryCount = m_count;
	ci.pipelineStatistics = m_pipelineStats;

	VDU_VK_CHECK_RESULT(vkCreateQueryPool(m_logicalDevice->getHandle(), &ci, 0, &m_queryPool));

	m_queryData = new uint64_t[m_count];
}

void vdu::QueryPool::destroy()
{
	vkDestroyQueryPool(m_logicalDevice->getHandle(), m_queryPool, nullptr);
}

void vdu::QueryPool::setQueryType(VkQueryType type)
{
	m_type = type;
}

void vdu::QueryPool::setPipelineStats(VkQueryPipelineStatisticFlags flags)
{
	m_pipelineStats = flags;
}

void vdu::QueryPool::setQueryCount(uint32_t count)
{
	m_count = count;
}

uint64_t * vdu::QueryPool::query()
{
	return query(0, m_count);
}

uint64_t * vdu::QueryPool::query(uint32_t first, uint32_t count)
{
	VDU_VK_CHECK_RESULT(vkGetQueryPoolResults(m_logicalDevice->getHandle(), m_queryPool, first, count, sizeof(uint64_t) * count, m_queryData, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT));
	return m_queryData;
}

void vdu::QueryPool::cmdReset(const vdu::CommandBuffer & cmd)
{
	cmdReset(cmd.getHandle(), 0, m_count);
}

void vdu::QueryPool::cmdReset(const vdu::CommandBuffer & cmd, uint32_t first, uint32_t count)
{
	cmdReset(cmd.getHandle(), first, count);
}

void vdu::QueryPool::cmdReset(const VkCommandBuffer & cmd)
{
	cmdReset(cmd, 0, m_count);
}

void vdu::QueryPool::cmdReset(const VkCommandBuffer & cmd, uint32_t first, uint32_t count)
{
	vkCmdResetQueryPool(cmd, m_queryPool, 0, count);
}

void vdu::QueryPool::cmdTimestamp(const vdu::CommandBuffer & cmd, VkPipelineStageFlagBits flags, uint32_t index)
{
	vkCmdWriteTimestamp(cmd.getHandle(), flags, m_queryPool, index);
}

void vdu::QueryPool::cmdTimestamp(const VkCommandBuffer & cmd, VkPipelineStageFlagBits flags, uint32_t index)
{
	vkCmdWriteTimestamp(cmd, flags, m_queryPool, index);
}
