#pragma once
#include "PCH.hpp"

namespace vdu
{
	template<typename T>
	T initializer()
	{
		DBG_SEVERE("Initializer doesn't exist");
		assert(false);
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
}