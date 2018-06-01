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
  
instance.addLayer("VK_LAYER_LUNARG_standard_validation");

instance.create();
```

## Enumerating physical devices and querying surface capabilities

```c++
// Physical device capabilities and features can be queried
std::vector<vdu::PhysicalDevice> allPhysicalDevices;

vdu::enumeratePhysicalDevices(instance, allPhysicalDevices);

VkSurfaceKHR surface;
allPhysicalDevices.front().querySurfaceCapabilities(surface)
```

## Creating queues and logical device with extensions, layers, and features
```c++
vdu::Queue graphicsQueue, transferQueue;

// Prepare queues with desired queue family and priority
// Queue families and their capabilities can be queried from PhysicalDevice
graphicsQueue.prepare(0, 1.f);
transferQueue.prepare(0, 1.f);

logicalDevice.addExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
logicalDevice.addLayer("VK_LAYER_LUNARG_standard_validation");
logicalDevice.addQueue(&lGraphicsQueue);
logicalDevice.addQueue(&lTransferQueue);

// Enable any device features you want
VkPhysicalDeviceFeatures pdf = {};
pdf.multiDrawIndirect = VK_TRUE;

logicalDevice.setEnabledDeviceFeatures(pdf);

logicalDevice.create(&physicalDevice);
```

## Creating descriptor sets
```c++
// First create the descriptor pool
descriptorPool.addPoolCount(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
descriptorPool.addPoolCount(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 20);
descriptorPool.addSetCount(1);

descriptorPool.create(&logicalDevice);

// Create the layout with some descriptor bindings
DescriptorSetLayout dsl;

dsl.addBinding("image_descriptor", vdu::DescriptorType::CombinedImageSampler, /*binding*/ 0, /*descriptor count*/ 20, vdu::ShaderStage::Fragment);
dsl.addBinding("buffer_descriptor", vdu::DescriptorType::UniformBuffer,       /*binding*/ 1, /*descriptor count*/ 1, vdu::ShaderStage::Fragment);

dsl.create(&logicalDevice);

// Finally create the descriptor set
DescriptorSet set;

set.create(&logicalDevice, &dsl, &pool);
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
