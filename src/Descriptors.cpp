#include "PCH.hpp"
#include "Descriptors.hpp"
#include "Initializers.hpp"

void vdu::DescriptorSetLayout::addBinding(VkDescriptorType type, uint32_t binding, uint32_t count, VkShaderStageFlags stageFlags)
{
	VkDescriptorSetLayoutBinding dslb = { binding , type, count, stageFlags };
	m_layoutBindings.push_back(dslb);
}

void vdu::DescriptorSetLayout::addBinding(DescriptorType type, uint32_t binding, uint32_t count, ShaderStage stageFlags)
{
	VkDescriptorSetLayoutBinding dslb = { binding , static_cast<VkDescriptorType>(type), count, static_cast<VkShaderStageFlags>(stageFlags) };
	m_layoutBindings.push_back(dslb);
}

void vdu::DescriptorSetLayout::create(LogicalDevice * logicalDevice)
{
	m_logicalDevice = logicalDevice;
	auto dslci = vdu::initializer<VkDescriptorSetLayoutCreateInfo>();
	dslci.bindingCount = m_layoutBindings.size();
	dslci.pBindings = m_layoutBindings.data();
	VDU_VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_logicalDevice->getHandle(), &dslci, nullptr, &m_descriptorSetLayout));
}

void vdu::DescriptorSetLayout::destroy()
{
	vkDestroyDescriptorSetLayout(m_logicalDevice->getHandle(), m_descriptorSetLayout, nullptr);
}
