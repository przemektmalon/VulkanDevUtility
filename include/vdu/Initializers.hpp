#pragma once
#include "PCH.hpp"

namespace vdu
{
	template<typename T>
	T initializer()
	{
		VDU_DBG_SEVERE("Initializer doesn't exist");
		assert(false);
		return T{};
	}

	template<>
	constexpr VkInstanceCreateInfo initializer<VkInstanceCreateInfo>()
	{
		VkInstanceCreateInfo ici{};
		ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		return ici;
	}

	template<>
	constexpr VkApplicationInfo initializer<VkApplicationInfo>()
	{
		VkApplicationInfo i{};
		i.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		return i;
	}

	template<>
	constexpr VkDebugReportCallbackCreateInfoEXT initializer<VkDebugReportCallbackCreateInfoEXT>()
	{
		VkDebugReportCallbackCreateInfoEXT drcci{};
		drcci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		return drcci;
	}

	template<>
	constexpr VkDeviceQueueCreateInfo initializer<VkDeviceQueueCreateInfo>()
	{
		VkDeviceQueueCreateInfo qci{};
		qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		return qci;
	}

	template<>
	constexpr VkDeviceCreateInfo initializer<VkDeviceCreateInfo>()
	{
		VkDeviceCreateInfo dci{};
		dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		return dci;
	}

	template<>
	constexpr VkDescriptorPoolCreateInfo initializer<VkDescriptorPoolCreateInfo>()
	{
		VkDescriptorPoolCreateInfo dci{};
		dci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		return dci;
	}

	template<>
	constexpr VkDescriptorSetLayoutCreateInfo initializer<VkDescriptorSetLayoutCreateInfo>()
	{
		VkDescriptorSetLayoutCreateInfo dslci{};
		dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		return dslci;
	}

	template<>
	constexpr VkDescriptorSetAllocateInfo initializer<VkDescriptorSetAllocateInfo>()
	{
		VkDescriptorSetAllocateInfo dsai{};
		dsai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		return dsai;
	}

	template<>
	constexpr VkWriteDescriptorSet initializer<VkWriteDescriptorSet>()
	{
		VkWriteDescriptorSet wds{};
		wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		return wds;
	}

	template<typename T>
	T initializer(size_t cs, uint32_t* pc) {}
	template<>
	constexpr VkShaderModuleCreateInfo initializer<VkShaderModuleCreateInfo>(size_t codeSize, uint32_t* pCode)
	{
		VkShaderModuleCreateInfo smci{};
		smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		smci.codeSize = codeSize;
		smci.pCode = pCode;
		return smci;
	}

	template<typename T>
	T initializer(VkShaderStageFlagBits fb, VkShaderModule m, const char* ep) {}
	template<>
	constexpr VkPipelineShaderStageCreateInfo initializer<VkPipelineShaderStageCreateInfo>(VkShaderStageFlagBits stage, VkShaderModule module, const char* entryPoint)
	{
		VkPipelineShaderStageCreateInfo pssci{};
		pssci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pssci.stage = stage;
		pssci.module = module;
		pssci.pName = entryPoint;
		return pssci;
	}
	
	template<>
	constexpr VkCommandPoolCreateInfo initializer<VkCommandPoolCreateInfo>()
	{
		VkCommandPoolCreateInfo cpci{};
		cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		return cpci;
	}

	template<>
	constexpr VkBufferCreateInfo initializer<VkBufferCreateInfo>()
	{
		VkBufferCreateInfo bci{};
		bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		return bci;
	}

}