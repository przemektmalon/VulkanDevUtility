#include "PCH.hpp"
#include "LogicalDevice.hpp"
#include "Initializers.hpp"
#include "Queue.hpp"

void vdu::LogicalDevice::create(PhysicalDevice* physicalDevice)
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

	VDU_VK_CHECK_RESULT(vkCreateDevice(m_physicalDevice->getHandle(), &dci, nullptr, &m_device));

	for (auto queue : m_queues)
	{
		VkQueue queueCreate;
		VDU_VK_VALIDATE(vkGetDeviceQueue(m_device, queue->getFamilyIndex(), queue->getIndex(), &queueCreate));
		queue->setQueueHandle(queueCreate);
	}
}

void vdu::LogicalDevice::destroy()
{
	VDU_VK_VALIDATE(vkDestroyDevice(m_device, nullptr));
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

void vdu::LogicalDevice::addExtension(const char * extensionName)
{
	m_enabledExtensions.push_back(extensionName);
}

void vdu::LogicalDevice::addLayer(const char * layerName)
{
	m_enabledLayers.push_back(layerName);
}
