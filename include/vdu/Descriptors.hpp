#pragma once
#include "PCH.hpp"
#include "Enums.hpp"
#include "LogicalDevice.hpp"
#include "MemoryPools.hpp"

namespace vdu
{
	class DescriptorSetLayout
	{
	public:

		void create(LogicalDevice* logicalDevice);

		void destroy();

		const VkDescriptorSetLayout& getHandle() const { return m_descriptorSetLayout; }

		void addBinding(const std::string& label, VkDescriptorType type, uint32_t binding, uint32_t count, VkShaderStageFlags stageFlags);
		void addBinding(const std::string& label, DescriptorTypeFlags type, uint32_t binding, uint32_t count, ShaderStageFlags stageFlags);

		const std::unordered_map<std::string, VkDescriptorSetLayoutBinding>& getLayoutBindingLabels() const { return m_layoutBindingsLabels; }
		const std::unordered_map<std::string, VkDescriptorSetLayoutBinding>& getImageLayoutBindings() const { return m_imageLayoutBindings; }
		const std::unordered_map<std::string, VkDescriptorSetLayoutBinding>& getBufferLayoutBindings() const { return m_bufferLayoutBindings; }

	private:

		VkDescriptorSetLayout m_descriptorSetLayout;
		std::unordered_map<std::string, VkDescriptorSetLayoutBinding> m_layoutBindingsLabels;
		std::unordered_map<std::string, VkDescriptorSetLayoutBinding> m_imageLayoutBindings;
		std::unordered_map<std::string, VkDescriptorSetLayoutBinding> m_bufferLayoutBindings;
		std::vector<VkDescriptorSetLayoutBinding> m_layoutBindings;
		LogicalDevice* m_logicalDevice;
	};

	class DescriptorSet
	{
	private:

		class SetUpdater
		{
		public:
			SetUpdater(DescriptorSet* dset);
			~SetUpdater();

			VkDescriptorImageInfo* addImageUpdate(const std::string& label, uint32_t arrayElement = 0, uint32_t count = 1);
			VkDescriptorBufferInfo* addBufferUpdate(const std::string& label, uint32_t arrayElement = 0, uint32_t count = 1);

			const std::vector<VkWriteDescriptorSet>& getWrites() { return m_writes; }

		private:

			std::vector<VkWriteDescriptorSet> m_writes;
			std::vector<VkDescriptorBufferInfo*> m_bufferInfos;
			std::vector<VkDescriptorImageInfo*> m_imageInfos;

			DescriptorSet* m_descriptorSet;
		};

	public:

		void create(LogicalDevice* logicalDevice, DescriptorSetLayout* layout, DescriptorPool* descriptorPool);

		void destroy();

		const VkDescriptorSet& getHandle() { return m_descriptorSet; }

		const DescriptorSetLayout* getLayout() { return m_descriptorSetLayout; }

		SetUpdater* makeUpdater();

		void destroyUpdater(SetUpdater* updater);

		void submitUpdater(SetUpdater* updater);

	private:

		VkDescriptorSet m_descriptorSet;
		DescriptorSetLayout* m_descriptorSetLayout;
		DescriptorPool* m_descriptorPool;
		LogicalDevice* m_logicalDevice;
	};
}