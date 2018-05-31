#pragma once
#include "PCH.hpp"
#include "Enums.hpp"
#include "LogicalDevice.hpp"

namespace vdu
{
	class DescriptorSetLayout
	{
	public:

		void create(LogicalDevice* logicalDevice);

		void destroy();

		const VkDescriptorSetLayout& getHandle() { return m_descriptorSetLayout; }

		void addBinding(VkDescriptorType type, uint32_t binding, uint32_t count, VkShaderStageFlags stageFlags);
		void addBinding(DescriptorType type, uint32_t binding, uint32_t count, ShaderStage stageFlags);

		

	private:

		VkDescriptorSetLayout m_descriptorSetLayout;
		std::vector<VkDescriptorSetLayoutBinding> m_layoutBindings;
		LogicalDevice* m_logicalDevice;
	};
}