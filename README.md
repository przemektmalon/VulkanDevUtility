# VulkanDevUtility
Vulkan development utilities, wrappers, initializers

## Creating Vulkan instance with extensions and layers

```c++
// Create Vulkan instance, add instance extensions and layers, debug callbacks and report levels

vdu::Instance instance;

instance.setApplicationName("App");
instance.setEngineName("Engine");
  
instance.addExtension(VK_KHR_SURFACE_EXTENSION_NAME);
instance.addExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
instance.addExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  
instance.setDebugCallbackFunction(debugCallbackFunc);
instance.addDebugReportLevel(vdu::Instance::DebugReportLevel::Warning);
instance.addDebugReportLevel(vdu::Instance::DebugReportLevel::Error);
instance.setDebugCallback([](
		vdu::Instance::DebugReportLevel level,
		vdu::Instance::DebugObjectType objectType,
		uint64_t objectHandle,
		const std::string& objectName,
		const std::string& message)
		-> void
{
    // Report error in some way
});
instance.addLayer("VK_LAYER_LUNARG_standard_validation");

instance.create();
```

## Enumerating physical devices and querying surface capabilities

```c++
// Get all Vulkan capable physical devices
// Physical device info, capabilities, and features can be queried from the vdu::PhysicalDevice class
std::vector<vdu::PhysicalDevice>& allPhysicalDevices = instance.enumratePhysicalDevices();

// Get the first device
vdu::PhysicalDevice* physDevice = allPhysicalDevices.front();

// Check if device has surface capabilities
physDevice.querySurfaceCapabilities( {your VkSurfaceKHR variable} ) // Surface creation not yet implemented
```

## Creating queues and logical device with extensions, layers, and features
```c++
vdu::Queue computeQueue;

std::vector<vdu::QueueFamily>& qFamilies = physDevice->getQueueFamilies();
for (auto& qFam : qFamilies) {
	if (qFam.supportsCompute()) {
		computeQueue = qFam.createQueue(1.f); // 1.f == queue priority
	}
}

// Create a logical device and add our queue(s)
vdu::LogicalDevice device;
device.addQueue(&computeQueue);
device.addExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
device.addLayer("VK_LAYER_LUNARG_standard_validation");

// Enable any device features you want
VkPhysicalDeviceFeatures pdf = {};
pdf.multiDrawIndirect = VK_TRUE;

device.setEnabledDeviceFeatures(pdf);
device.create(&physicalDevice);
```

## Creating descriptor sets
```c++
// First create the descriptor pool
vdu::DescriptorPool descPool;
descPool.addPoolCount(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
descPool.addPoolCount(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 20);
descPool.addSetCount(1);

descPool.create(&device);

// Create the layout with some descriptor bindings
vdu::DescriptorSetLayout dsl;

dsl.addBinding("image_descriptor", vdu::DescriptorType::CombinedImageSampler, /*binding*/ 0, /*descriptor count*/ 20, vdu::ShaderStage::Fragment);
dsl.addBinding("buffer_descriptor", vdu::DescriptorType::UniformBuffer,       /*binding*/ 1, /*descriptor count*/ 1, vdu::ShaderStage::Fragment);

dsl.create(&device);

// Finally create the descriptor set
vdu::DescriptorSet set;

set.create(&device, &dsl, &pool);
```

## Updating descriptor sets
```c++
// When we want to update the set, we make a reusable updater

SetUpdater* updater = set.makeUpdater();

// Update one buffer
auto buffer_descriptor_update = updater->addBufferUpdate("buffer_descriptor"); // the label is the same as that in the layout
buffer_descriptor_update->buffer = /* VkBuffer */;
buffer_descriptor_update->offset = /* offset of update */;
buffer_descriptor_update->range = /* VK_WHOLE_RANGE or the range you want */

// Update the 20 images (using optional shorthand)
auto image_descriptor_update = updater->addImageUpdate("image_descriptor");
for (int i = 0; i < 20; ++i)
{
  image_descriptor_update[i] = { /* Sampler */ , /* Image View */ , /* Image Layout */ };
}

// Finally submit out updates
set.submitUpdater(updater);

// We can keep the updater to re-use another time or destroy it
set.destroyUpdater(updater);
```
