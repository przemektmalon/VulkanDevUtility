#include "PCH.hpp"
#include "MemoryPools.hpp"
#include "Initializers.hpp"

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
