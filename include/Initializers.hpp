#pragma once
#include "PCH.hpp"

namespace vdu
{
	template<typename T>
	T initializer()
	{
		printf("Initializer doesn't exist");
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
}