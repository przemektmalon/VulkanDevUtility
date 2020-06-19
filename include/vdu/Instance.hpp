#pragma once
#include "Debug.hpp"
#include "PCH.hpp"
#include "PhysicalDevice.hpp"

namespace vdu {
class Buffer;

/*
        Wrapper for vulkan instance
        */
class Instance {
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
          Object types for debug report callback
          */
  enum DebugObjectType {
    Unknown,
    Instance_, // Cant have same name as class Instance, hence the _
    PhysicalDevice,
    Device,
    Queue,
    Semaphore,
    CommandBuffer,
    Fence,
    DeviceMemory,
    Buffer,
    Image,
    Event,
    QueryPool,
    BufferView,
    ImageView,
    ShaderModule,
    PipelineCache,
    PipelinLayout,
    RenderPass,
    Pipeline,
    DescriptorSetLayout,
    Sampler,
    DescriptorPool,
    DescriptorSet,
    Framebuffer,
    CommandPool,
    Surface,
    Swapchain,
    DebugReport,
    Display,
    DisplayMode,
    ObjectTableNVX,
    IndirectCommandsLayoutNVX
  };

  /*
          Define type of function for debug callbacks
          */
  typedef void (*PFN_vkDebugCallback)(DebugReportLevel level,
                                      DebugObjectType objectType,
                                      uint64_t objectHandle,
                                      const std::string &objectName,
                                      const std::string &message);

  /*
          Initialised with some default values
          */
  Instance();

  /*
          Create the instance with previously specified extensions, layers, and
     other creation data
          */
  VkResult create();

  /*
          Destroy the instance
          */
  void destroy();

  /*
          Add an extension
          */
  void addExtension(const char *extensionName);

  /*
          Add a layer
          */
  void addLayer(const char *layerName);

  /*
          Add a debug report level (OR'd with previous value)
          */
  void addDebugReportLevel(DebugReportLevel debugReportLevel);

  /*
          Set application name
          */
  void setApplicationName(const std::string &appName);

  /*
          Set engine name
          */
  void setEngineName(const std::string &engineName);

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
          Populate a list of vulkan capable physical devices with their
     properties fields filled out Return a reference to this list
          */
  std::vector<vdu::PhysicalDevice> &enumratePhysicalDevices();

  /*
          Get a reference to the physical devices
          */
  std::vector<vdu::PhysicalDevice> &getPhysicalDevices();

  void nameObject(vdu::Buffer *buffer, const std::string &name);

private:
  /*
          Vulkan instance
          */
  VkInstance m_instance;

  /*
          Enabled layers and extensions
          */
  std::vector<const char *> m_enabledExtensions;
  std::vector<const char *> m_enabledLayers;

  /*
          Physical devices
          */
  std::vector<vdu::PhysicalDevice> m_physicalDevices;

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
          Debug callback vulkan handle
          */
  VkDebugReportCallbackEXT m_debugReportCallback;

  /*
          Library user debug callback function
          */
  PFN_vkDebugCallback m_userDebugCallbackFunc;

  /*
          Used in 'pUserData' parameter to vkCreateDebugReportCallbackEXT in
     order to identify the vdu::Instance that the callback belongs to
          */
  Instance *m_thisInstance;

public:
  void setDebugCallback(PFN_vkDebugCallback callback);
  PFN_vkDebugCallback getDebugCallbackFunc() { return m_userDebugCallbackFunc; }

  vdu::ObjectNamer m_objectNamer;
};
} // namespace vdu

/*
Vulkan debug callback function
*/
VKAPI_ATTR VkBool32 VKAPI_CALL vduVkDebugCallbackFunc(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
    uint64_t obj, size_t location, int32_t code, const char *layerPrefix,
    const char *msg, void *userData);
