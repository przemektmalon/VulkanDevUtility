#include "LogicalDevice.hpp"
#include "Initializers.hpp"
#include "PCH.hpp"
#include "PhysicalDevice.hpp"
#include "Queue.hpp"

VkResult vdu::LogicalDevice::create(PhysicalDevice *physicalDevice) {
  m_physicalDevice = physicalDevice;

  std::vector<VkDeviceQueueCreateInfo> qcis;
  for (auto &familyCount : m_queueFamilyCountsPriorities) {
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

  auto result =
      vkCreateDevice(m_physicalDevice->getHandle(), &dci, nullptr, &m_device);
  if (result != VK_SUCCESS)
    return result;

  for (auto queue : m_queues) {
    VkQueue queueCreate;
    vkGetDeviceQueue(m_device, queue->getFamily()->getIndex(),
                     queue->getIndex(), &queueCreate);
    queue->setQueueHandle(queueCreate);
  }
  return VK_SUCCESS;
}

void vdu::LogicalDevice::destroy() { vkDestroyDevice(m_device, nullptr); }

void vdu::LogicalDevice::addQueue(Queue *queue) {
  auto ins = m_queues.insert(queue);
  if (ins.second) {
    queue->setIndex(
        m_queueFamilyCountsPriorities[queue->getFamily()->getIndex()].size());
    m_queueFamilyCountsPriorities[queue->getFamily()->getIndex()].push_back(
        queue->getPriority());
  }
}

void vdu::LogicalDevice::setEnabledDeviceFeatures(
    const VkPhysicalDeviceFeatures &pdf) {
  m_enabledDeviceFeatures = pdf;
}

void vdu::LogicalDevice::setVkErrorCallback(PFN_vkErrorCallback errorCallback) {
  m_vkErrorCallbackFunc = errorCallback;
}

void vdu::LogicalDevice::setVduDebugCallback(
    PFN_vduDebugCallback errorCallback) {
  m_vduDebugCallbackFunc = errorCallback;
}

void vdu::LogicalDevice::_internalReportVkError(VkResult error,
                                                const std::string &message) {
  if (m_vkErrorCallbackFunc)
    m_vkErrorCallbackFunc(error, message);
  else
    assert(false); // If you are here then set an error callback and handle the
                   // error (or not and get crashes)
}

void vdu::LogicalDevice::_internalReportVduDebug(VduDebugLevel level,
                                                 const std::string &message) {
  if (m_vduDebugCallbackFunc)
    m_vduDebugCallbackFunc(level, message);
  else
    assert(false); // If you are here then set an error callback and handle the
                   // error (or not and get crashes)
}

void vdu::LogicalDevice::addExtension(const char *extensionName) {
  m_enabledExtensions.push_back(extensionName);
}

void vdu::LogicalDevice::addLayer(const char *layerName) {
  m_enabledLayers.push_back(layerName);
}
