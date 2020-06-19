#include "Instance.hpp"
#include "PCH.hpp"
#include "VDU.hpp"

#define VK_CHECK_RESULT(f)                                                     \
  {                                                                            \
    auto result = f;                                                           \
    if (result != VK_SUCCESS)                                                  \
      return result;                                                           \
  }

vdu::Instance::Instance()
    : m_instance(0), m_debugReportLevel(0), m_debugReportCallback(nullptr),
      m_applicationName(""), m_engineName(""), m_apiVersion(VK_API_VERSION_1_0),
      m_engineVersion(VK_MAKE_VERSION(1, 0, 0)),
      m_applicationVersion(VK_MAKE_VERSION(1, 0, 0)), m_thisInstance(this) {}

VkResult vdu::Instance::create() {
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

  auto result = vkCreateInstance(&instInfo, nullptr, &m_instance);
  if (result != VK_SUCCESS) {
    std::cout << "Failed to create instance. Error code: " << result << "\n";
    return result;
  }

  if (m_userDebugCallbackFunc != nullptr) {
    auto drcci = vdu::initializer<VkDebugReportCallbackCreateInfoEXT>();
    drcci.flags = m_debugReportLevel;
    drcci.pfnCallback = &vduVkDebugCallbackFunc;
    drcci.pUserData = &m_thisInstance;
    auto createDebugReportCallbackEXT =
        (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
            m_instance, "vkCreateDebugReportCallbackEXT");

    result = createDebugReportCallbackEXT(m_instance, &drcci, nullptr,
                                          &m_debugReportCallback);
  }

  return result;
}

void vdu::Instance::destroy() {
  if (m_userDebugCallbackFunc != nullptr) {
    PFN_vkDestroyDebugReportCallbackEXT(
        vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT"))(
        m_instance, m_debugReportCallback, 0);
  }
  vkDestroyInstance(m_instance, nullptr);
}

void vdu::Instance::addExtension(const char *extensionName) {
  m_enabledExtensions.push_back(extensionName);
}

void vdu::Instance::addLayer(const char *layerName) {
  m_enabledLayers.push_back(layerName);
}

void vdu::Instance::addDebugReportLevel(DebugReportLevel debugReportLevel) {
  m_debugReportLevel |= debugReportLevel;
}

void vdu::Instance::setApplicationName(const std::string &appName) {
  m_applicationName = appName;
}

void vdu::Instance::setEngineName(const std::string &engineName) {
  m_engineName = engineName;
}

void vdu::Instance::setApplicationVersion(int32_t major, int32_t minor,
                                          int32_t patch) {
  m_applicationVersion = VK_MAKE_VERSION(major, minor, patch);
}

void vdu::Instance::setEngineVersion(int32_t major, int32_t minor,
                                     int32_t patch) {
  m_engineVersion = VK_MAKE_VERSION(major, minor, patch);
}

void vdu::Instance::setVulkanVersion(int32_t major, int32_t minor,
                                     int32_t patch) {
  m_apiVersion = VK_MAKE_VERSION(major, minor, patch);
}

VkInstance vdu::Instance::getInstanceHandle() { return m_instance; }

std::vector<vdu::PhysicalDevice> &vdu::Instance::enumratePhysicalDevices() {
  // Get the physical device count
  uint32_t deviceCount = 0;
  if (vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr) !=
      VK_SUCCESS) {
    assert(false);
  }

  // Initialise vectors
  std::vector<VkPhysicalDevice> physicalDeviceHandles;
  physicalDeviceHandles.resize(deviceCount);
  m_physicalDevices.reserve(deviceCount);

  // Get actual physical device handles
  if (vkEnumeratePhysicalDevices(m_instance, &deviceCount,
                                 physicalDeviceHandles.data()) != VK_SUCCESS) {
    assert(false);
  }

  // For each handle add it to our device list and query (fill in) its details
  for (auto physicalDevice : physicalDeviceHandles) {
    m_physicalDevices.emplace_back(
        physicalDevice); // Constructor queries all device details
  }
  return m_physicalDevices;
}

std::vector<vdu::PhysicalDevice> &vdu::Instance::getPhysicalDevices() {
  return m_physicalDevices;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vduVkDebugCallbackFunc(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
    uint64_t obj, size_t location, int32_t code, const char *layerPrefix,
    const char *msg, void *userData) {
  vdu::Instance *instance = *(vdu::Instance **)userData;

  auto debugCallbackFunc = instance->getDebugCallbackFunc();

  if (debugCallbackFunc) {
    std::string message = msg;
    std::string objectName;
    objectName.reserve(128);

    switch (objType) {
    case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT:
      objectName.append(instance->m_objectNamer.getName(VkBuffer(obj)));
      break;
    }

    debugCallbackFunc((vdu::Instance::DebugReportLevel)flags,
                      (vdu::Instance::DebugObjectType)objType, obj, objectName,
                      message);
  }

  return 0;
}

void vdu::Instance::nameObject(vdu::Buffer *buffer, const std::string &name) {
  m_objectNamer.addName(buffer->getHandle(), name);
}

void vdu::Instance::setDebugCallback(PFN_vkDebugCallback callback) {
  m_userDebugCallbackFunc = callback;
}
