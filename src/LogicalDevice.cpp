#include "PCH.hpp"
#include "LogicalDevice.hpp"
#include "Initializers.hpp"
#include "Queue.hpp"

VkResult vdu::LogicalDevice::create(PhysicalDevice* physicalDevice)
{
	m_physicalDevice = physicalDevice;

	std::vector<VkDeviceQueueCreateInfo> qcis;
	for (auto& familyCount : m_queueFamilyCountsPriorities)
	{
		auto qci = vdu::initializer<VkDeviceQueueCreateInfo>();
		qci.queueFamilyIndex = familyCount.first;
		qci.queueCount = familyCount.second.size();
		qci.pQueuePriorities = familyCount.second.data();
		qcis.push_back(qci);
	}

	auto dci = vdu::initializer<VkDeviceCreateInfo>();
	dci.queueCreateInfoCount = qcis.size();
	dci.pQueueCreateInfos = qcis.data();
	dci.enabledLayerCount = m_enabledLayers.size();
	dci.ppEnabledLayerNames = m_enabledLayers.data();
	dci.enabledExtensionCount = m_enabledExtensions.size();
	dci.ppEnabledExtensionNames = m_enabledExtensions.data();
	dci.pEnabledFeatures = &m_enabledDeviceFeatures;

	auto result = vkCreateDevice(m_physicalDevice->getHandle(), &dci, nullptr, &m_device);
	if (result != VK_SUCCESS)
		return result;

	for (auto queue : m_queues)
	{
		VkQueue queueCreate;
		vkGetDeviceQueue(m_device, queue->getFamilyIndex(), queue->getIndex(), &queueCreate);
		queue->setQueueHandle(queueCreate);
	}
	return VK_SUCCESS;
}

void vdu::LogicalDevice::destroy()
{
	vkDestroyDevice(m_device, nullptr);
}

void vdu::LogicalDevice::addQueue(Queue * queue)
{
	auto ins = m_queues.insert(queue);
	if (ins.second)
	{
		queue->setIndex(m_queueFamilyCountsPriorities[queue->getFamilyIndex()].size());
		m_queueFamilyCountsPriorities[queue->getFamilyIndex()].push_back(queue->getPriority());
	}
}

void vdu::LogicalDevice::setEnabledDeviceFeatures(const VkPhysicalDeviceFeatures & pdf)
{
	m_enabledDeviceFeatures = pdf;
}

void vdu::LogicalDevice::setErrorCallback(PFN_vkErrorCallback errorCallback)
{
	m_errorCallbackFunc = errorCallback;
}

void vdu::LogicalDevice::_internalReportError(VkResult error, const std::string & message)
{
	if (m_errorCallbackFunc)
		m_errorCallbackFunc(error, message);
	else
		assert(false); // If you are here then set an error callback and handle the error (or not and get crashes)
}

void vdu::LogicalDevice::addExtension(const char * extensionName)
{
	m_enabledExtensions.push_back(extensionName);
}

void vdu::LogicalDevice::addLayer(const char * layerName)
{
	m_enabledLayers.push_back(layerName);
}
