#include "PCH.hpp"
#include "Instance.hpp"
#include "VDU.hpp"

vdu::Instance::Instance() :
	m_instance(0),
	m_debugReportLevel(0),
	m_applicationName(""),
	m_engineName(""),
	m_apiVersion(VK_API_VERSION_1_0),
	m_engineVersion(VK_MAKE_VERSION(1, 0, 0)),
	m_applicationVersion(VK_MAKE_VERSION(1, 0, 0)),
	m_debugReportCallbackFunction(nullptr)
{
}

void vdu::Instance::create()
{
	vdu::internal_debugging_instance = this;

	auto appInfo = vdu::initializer<VkApplicationInfo>();

	appInfo.apiVersion = m_apiVersion;
	appInfo.applicationVersion = m_applicationVersion;
	appInfo.engineVersion = m_engineVersion;

	appInfo.pApplicationName = m_applicationName.c_str();
	appInfo.pEngineName = m_engineName.c_str();


	auto instInfo = vdu::initializer<VkInstanceCreateInfo>();

	instInfo.pApplicationInfo = &appInfo;

	instInfo.enabledExtensionCount = m_enabledExtensions.size();
	instInfo.ppEnabledExtensionNames = m_enabledExtensions.data();

	instInfo.enabledLayerCount = m_enabledLayers.size();
	instInfo.ppEnabledLayerNames = m_enabledLayers.data();

	VDU_VK_CHECK_RESULT(vkCreateInstance(&instInfo, nullptr, &m_instance));

#ifdef VDU_WITH_VALIDATION
	if (m_debugReportCallbackFunction)
	{
		auto drcci = vdu::initializer<VkDebugReportCallbackCreateInfoEXT>();
		drcci.flags = m_debugReportLevel;
		drcci.pfnCallback = &debugCallbackFunc;
		auto createDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT");

		VDU_VK_CHECK_RESULT(createDebugReportCallbackEXT(m_instance, &drcci, nullptr, &m_debugReportCallback));
	}
#endif
}

void vdu::Instance::destroy()
{
	if (m_debugReportCallbackFunction)
	{
		PFN_vkDestroyDebugReportCallbackEXT(vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT"))(m_instance, m_debugReportCallback, 0);
	}
	VDU_VK_VALIDATE(vkDestroyInstance(m_instance, nullptr));
}

void vdu::Instance::addExtension(const char * extensionName)
{
	m_enabledExtensions.push_back(extensionName);
}

void vdu::Instance::addLayer(const char * layerName)
{
	m_enabledLayers.push_back(layerName);
}

void vdu::Instance::setDebugCallbackFunction(PFN_vkDebugReportCallbackEXT debugCallbackFunction)
{
	m_debugReportCallbackFunction = debugCallbackFunction;
}

void vdu::Instance::addDebugReportLevel(DebugReportLevel debugReportLevel)
{
	m_debugReportLevel |= debugReportLevel;
}

void vdu::Instance::setApplicationName(const std::string & appName)
{
	m_applicationName = appName;
}

void vdu::Instance::setEngineName(const std::string & engineName)
{
	m_engineName = engineName;
}

void vdu::Instance::setApplicationVersion(int32_t major, int32_t minor, int32_t patch)
{
	m_applicationVersion = VK_MAKE_VERSION(major, minor, patch);
}

void vdu::Instance::setEngineVersion(int32_t major, int32_t minor, int32_t patch)
{
	m_engineVersion = VK_MAKE_VERSION(major, minor, patch);
}

void vdu::Instance::setVulkanVersion(int32_t major, int32_t minor, int32_t patch)
{
	m_apiVersion = VK_MAKE_VERSION(major, minor, patch);
}

VkInstance vdu::Instance::getInstanceHandle()
{
	return m_instance;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vdu::Instance::debugCallbackFunc(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char * layerPrefix, const char * msg, void * userData)
{
	m_validationWarning = true;
	m_validationMessage = msg;
	if (vdu::internal_debugging_instance->m_debugReportCallbackFunction)
		return vdu::internal_debugging_instance->m_debugReportCallbackFunction(flags, objType, obj, location, code, layerPrefix, msg, userData);
	return 0;
}

thread_local bool vdu::Instance::m_validationWarning;
thread_local std::string vdu::Instance::m_validationMessage;
thread_local VkResult vdu::Instance::m_lastVulkanResult;