#include "PCH.hpp"
#include "Descriptors.hpp"
#include "Initializers.hpp"

void vdu::DescriptorSetLayout::addBinding(const std::string& label, VkDescriptorType type, uint32_t binding, uint32_t count, VkShaderStageFlags stageFlags)
{
	VkDescriptorSetLayoutBinding dslb = { binding , type, count, stageFlags };
	m_layoutBindingsLabels.insert(std::make_pair(label, dslb));
	m_layoutBindings.push_back(dslb);
	switch (type)
	{
	case VK_DESCRIPTOR_TYPE_SAMPLER:
	case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
	case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
	case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
	case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
		m_imageLayoutBindings.insert(std::make_pair(label, dslb));
		break;
	case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
	case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
	case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
	case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
	case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
	case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
		m_bufferLayoutBindings.insert(std::make_pair(label, dslb));
		break;
	}
}

void vdu::DescriptorSetLayout::addBinding(const std::string& label, DescriptorTypeFlags type, uint32_t binding, uint32_t count, ShaderStageFlags stageFlags)
{
	addBinding(label, static_cast<VkDescriptorType>(type), binding, count, static_cast<VkShaderStageFlags>(stageFlags));
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
	VDU_VK_VALIDATE(vkDestroyDescriptorSetLayout(m_logicalDevice->getHandle(), m_descriptorSetLayout, nullptr));
}

vdu::DescriptorSet::SetUpdater::SetUpdater(DescriptorSet * dset)
{
	auto layout = dset->getLayout();
	const auto& allBindings = layout->getLayoutBindingLabels();
	const auto& imageBindings = layout->getImageLayoutBindings();
	const auto& bufferBindings = layout->getBufferLayoutBindings();

	m_writes.reserve(allBindings.size());
	m_imageInfos.reserve(imageBindings.size());
	m_bufferInfos.reserve(bufferBindings.size());

	m_descriptorSet = dset;
}

vdu::DescriptorSet::SetUpdater::~SetUpdater()
{
	for (auto imageInfo : m_imageInfos)
	{
		delete[] imageInfo;
	}
	for (auto bufferInfo : m_bufferInfos)
	{
		delete[] bufferInfo;
	}
}

VkDescriptorImageInfo * vdu::DescriptorSet::SetUpdater::addImageUpdate(const std::string & label, uint32_t arrayElement, uint32_t count)
{
	auto dii = new VkDescriptorImageInfo[count];
	m_imageInfos.push_back(dii);
	auto ret = m_imageInfos.back();

	const auto& allBindings = m_descriptorSet->getLayout()->getLayoutBindingLabels();

	auto bindingFind = allBindings.find(label);
	if (bindingFind == allBindings.end())
		DBG_SEVERE("Attempting to update a descriptor label that doesnt exist");

	auto wds = vdu::initializer<VkWriteDescriptorSet>();
	wds.dstSet = m_descriptorSet->getHandle();
	wds.dstBinding = bindingFind->second.binding;
	wds.descriptorType = bindingFind->second.descriptorType;
	wds.dstArrayElement = arrayElement;
	wds.descriptorCount = count;
	wds.pImageInfo = ret;
	m_writes.push_back(wds);

	return ret;
}

VkDescriptorBufferInfo * vdu::DescriptorSet::SetUpdater::addBufferUpdate(const std::string & label, uint32_t arrayElement, uint32_t count)
{
	auto dbi = new VkDescriptorBufferInfo[count];
	m_bufferInfos.push_back(dbi);
	auto ret = m_bufferInfos.back();

	const auto& allBindings = m_descriptorSet->getLayout()->getLayoutBindingLabels();

	auto bindingFind = allBindings.find(label);
	if (bindingFind == allBindings.end())
		DBG_SEVERE("Attempting to update a descriptor label that doesnt exist");

	auto wds = vdu::initializer<VkWriteDescriptorSet>();
	wds.dstSet = m_descriptorSet->getHandle();
	wds.dstBinding = bindingFind->second.binding;
	wds.descriptorType = bindingFind->second.descriptorType;
	wds.dstArrayElement = arrayElement;
	wds.descriptorCount = count;
	wds.pBufferInfo = ret;
	m_writes.push_back(wds);

	return ret;
}

void vdu::DescriptorSet::create(LogicalDevice * logicalDevice, DescriptorSetLayout * layout, DescriptorPool* descriptorPool)
{
	m_logicalDevice = logicalDevice;
	m_descriptorSetLayout = layout;
	m_descriptorPool = descriptorPool;

	auto dsai = vdu::initializer<VkDescriptorSetAllocateInfo>();
	dsai.descriptorPool = m_descriptorPool->getHandle();
	dsai.descriptorSetCount = 1;
	dsai.pSetLayouts = &m_descriptorSetLayout->getHandle();

	VDU_VK_CHECK_RESULT(vkAllocateDescriptorSets(m_logicalDevice->getHandle(), &dsai, &m_descriptorSet));
}

void vdu::DescriptorSet::destroy()
{
	VDU_VK_CHECK_RESULT(vkFreeDescriptorSets(m_logicalDevice->getHandle(), m_descriptorPool->getHandle(), 1, &m_descriptorSet));
}

vdu::DescriptorSet::SetUpdater * vdu::DescriptorSet::makeUpdater()
{
	return new SetUpdater(this);
}

void vdu::DescriptorSet::destroyUpdater(SetUpdater * updater)
{
	delete updater;
}

void vdu::DescriptorSet::submitUpdater(SetUpdater * updater)
{
	auto& writes = updater->getWrites();
	VDU_VK_VALIDATE(vkUpdateDescriptorSets(m_logicalDevice->getHandle(), writes.size(), writes.data(), 0, nullptr));
}
