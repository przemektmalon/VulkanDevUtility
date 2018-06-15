#pragma once
#include "PCH.hpp"

namespace vdu
{
	/*
	Wrapper for vulkan instance
	*/
	class Instance
	{
	public:
		/*
		Levels of debug report callback (for validation layers)
		*/
		enum DebugReportLevel {
			Information = 1,
			Warning = 2,
			Performance = 4,
			Error = 8,
			Debug = 16
		};

		/*
		Initialised with some default values
		*/
		Instance();

		/*
		Create the instance with previously specified extensions, layers, and other creation data
		*/
		void create();

		/*
		Destroy the instance
		*/
		void destroy();

		/*
		Add an extension
		*/
		void addExtension(const char * extensionName);

		/*
		Add a layer
		*/
		void addLayer(const char * layerName);

		/*
		Set the debug report callback function
		*/
		void setDebugCallbackFunction(PFN_vkDebugReportCallbackEXT debugCallbackFunction);

		/*
		Add a debug report level (OR'd with previous value)
		*/
		void addDebugReportLevel(DebugReportLevel debugReportLevel);

		/*
		Set application name
		*/
		void setApplicationName(const std::string& appName);

		/*
		Set engine name
		*/
		void setEngineName(const std::string& engineName);

		/*
		Set application version
		*/
		void setApplicationVersion(int32_t major, int32_t minor, int32_t patch);

		/*
		Set engine version
		*/
		void setEngineVersion(int32_t major, int32_t minor, int32_t patch);

		/*
		Set Vulkan API version
		*/
		void setVulkanVersion(int32_t major, int32_t minor, int32_t patch);

		/*
		Get vkInstance handle
		*/
		VkInstance getInstanceHandle();

		/*
		Debug callback function
		*/
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallbackFunc(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData);


	private:

		/*
		Vulkan instance
		*/
		VkInstance m_instance;

		/*
		Enabled layers and extensions
		*/
		std::vector<const char*> m_enabledExtensions;
		std::vector<const char*> m_enabledLayers;

		/*
		Application/engine/api versions names
		*/
		std::string m_applicationName;
		std::string m_engineName;

		uint32_t m_apiVersion;
		uint32_t m_engineVersion;
		uint32_t m_applicationVersion;

		/*
		Level of debug report callback (combination of enum DebugReportLevel)
		*/
		int32_t m_debugReportLevel;

		/*
		Debug callback function pointer
		*/
		PFN_vkDebugReportCallbackEXT m_debugReportCallbackFunction;

		/*
		Debug callback vulkan handle
		*/
		VkDebugReportCallbackEXT m_debugReportCallback;

	public:

		/*
		Flag and message for a validation warnings/errors
		*/
		static thread_local bool m_validationWarning;
		static thread_local std::string m_validationMessage;
		static thread_local VkResult m_lastVulkanResult;
	};

	static vdu::Instance* internal_debugging_instance;
}

