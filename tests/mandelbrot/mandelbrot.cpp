#define VK_USE_PLATFORM_XCB_KHR
#include "VDU.hpp"
#include <iostream>
#include <cmath>
#include <string.h>

constexpr uint32_t resX = 1920;
constexpr uint32_t resY = 1080;
constexpr uint32_t iterations = 5000;

void saveBitmapToFile(const char *file, unsigned char *bitmap, int width, int height);

int main()
{
	// Setup the instance and validation layer callback
	vdu::Instance instance;
	instance.setApplicationName("Mandelbrot");
	instance.addExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	instance.addDebugReportLevel(vdu::Instance::DebugReportLevel::Warning);
	instance.addDebugReportLevel(vdu::Instance::DebugReportLevel::Error);
	instance.setDebugCallback([](
								  vdu::Instance::DebugReportLevel level,
								  vdu::Instance::DebugObjectType objectType,
								  uint64_t objectHandle,
								  const std::string &objectName,
								  const std::string &message)
								  -> void {
		std::cout << message << std::endl
				  << std::endl;
	});
	instance.addLayer("VK_LAYER_LUNARG_standard_validation");
	instance.create();

	std::cout << "Number of Vulkan capable devices: " << instance.enumratePhysicalDevices().size() << "\n";

	// Choose the first physical device available
	vdu::PhysicalDevice *physicalDevice = &instance.enumratePhysicalDevices().front();

	std::cout << "Vulkan device used: " << physicalDevice->getDeviceProperties().deviceName << "\n";

	// We're going to choose queues that support the necessary operations
	vdu::Queue computeQueue;
	vdu::Queue transferQueue;

	std::vector<vdu::QueueFamily> &qFamilies = physicalDevice->getQueueFamilies();

	for (auto &qFam : qFamilies)
	{
		if (qFam.supportsCompute())
		{
			computeQueue = qFam.createQueue(1.f); // Queue priority = 1.f
		}
		if (qFam.supportsTransfer())
		{
			transferQueue = qFam.createQueue(1.f);
		}
	}

	// Create a logical device and add our queue(s)
	vdu::LogicalDevice device;
	device.addQueue(&computeQueue);
	if (computeQueue.sameFamilyAs(transferQueue))
	{
		device.create(physicalDevice);
		transferQueue = computeQueue;
	}
	else
	{
		device.addQueue(&transferQueue);
		device.create(physicalDevice);
	}

	// Setup a VDU callback (use example: shader compilation errors)
	device.setVduDebugCallback([](vdu::LogicalDevice::VduDebugLevel level, const std::string &message) -> void {
		std::cout << message << std::endl;
	});

	// Setup a Vulkan error callback (for when internal VkResult is not VK_SUCCESS)
	device.setVkErrorCallback([](VkResult error, const std::string &message) -> void {
		std::cout << message << std::endl;
	});

	// Create and compile our mandelbrot shader
	vdu::ShaderProgram shader;
	shader.addModule(vdu::ShaderStage::Compute, "mandelbrot.comp");
	shader.create(&device);
	shader.compile();

	// Create a texture with desired dimensions, format, and usage
	vdu::TextureCreateInfo tci;
	tci.width = resX;
	tci.height = resY;
	tci.format = VK_FORMAT_R8G8B8A8_UNORM;
	tci.usageFlags = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	vdu::Texture outputTexture;
	outputTexture.setProperties(tci);
	outputTexture.create(&device);

	// Create descriptor pool, layout, and set
	vdu::DescriptorPool descPool;
	descPool.addPoolCount(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1);
	descPool.addSetCount(1);
	descPool.create(&device);

	// The name of the binding will be used in descriptor updates!
	vdu::DescriptorSetLayout descSetLayout;
	descSetLayout.addBinding("output", vdu::DescriptorType::StorageImage, 0, 1, vdu::ShaderStage::Compute);
	descSetLayout.create(&device);

	vdu::DescriptorSet descSet;
	descSet.allocate(&device, &descSetLayout, &descPool);

	// Create a texture sampler (no VDU class for this, yet)
	VkSampler texSampler;
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	vkCreateSampler(device.getHandle(), &samplerInfo, nullptr, &texSampler);

	// Update the descriptor set
	auto updater = descSet.makeUpdater();
	auto imageUpdate = updater->addImageUpdate("output"); // Name is the same as in descriptor set layout
	imageUpdate->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imageUpdate->imageView = outputTexture.getView();
	imageUpdate->sampler = texSampler;
	descSet.submitUpdater(updater);
	descSet.destroyUpdater(updater);

	// Create command pool, we may need two if the compute and transfer queues are not the same
	vdu::CommandPool cmdPool, cmdPool2;
	cmdPool.setQueueFamily(computeQueue.getFamily());
	cmdPool.create(&device);
	if (computeQueue.sameFamilyAs(transferQueue))
	{
		cmdPool2 = cmdPool;
	}
	else
	{
		cmdPool2.setQueueFamily(transferQueue.getFamily());
		cmdPool2.create(&device);
	}

	// Allocate command buffer for draw commands
	vdu::CommandBuffer drawCommands;
	drawCommands.allocate(&device, &cmdPool);

	// Create pipeline layout with our descriptor set layout and push constant range for storing resolution and number of iterations
	vdu::PipelineLayout pipelineLayout;
	pipelineLayout.addDescriptorSetLayout(&descSetLayout);
	pipelineLayout.addPushConstantRange(vdu::PushConstantRange{vdu::ShaderStage::Compute, 0u, 12u});
	pipelineLayout.create(&device);

	// Create a compute pipeline with our layout and shader
	vdu::ComputePipeline pipeline;
	pipeline.setPipelineLayout(&pipelineLayout);
	pipeline.setShaderProgram(&shader);
	pipeline.create(&device);

	// Write our draw commands
	drawCommands.begin();
	auto cmd = drawCommands.getHandle();

	outputTexture.cmdTransitionLayout(drawCommands, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.getHandle());
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout.getHandle(), 0, 1, &descSet.getHandle(), 0, 0);

	// Push constant data
	uint32_t pushConstData[3];
	pushConstData[0] = resX;
	pushConstData[1] = resY;
	pushConstData[2] = iterations;

	vkCmdPushConstants(cmd, pipelineLayout.getHandle(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pushConstData), pushConstData);

	vkCmdDispatch(cmd, std::ceil(resX / 16), std::ceil(resY / 16), 1);

	drawCommands.end();

	// Semaphore to signal transfer operation
	vdu::Semaphore sem;
	sem.create(&device);

	// Submit our commands
	vdu::QueueSubmission drawMandelbrot;
	drawMandelbrot.addCommands(cmd);
	drawMandelbrot.addSignal(sem);
	computeQueue.submit(drawMandelbrot);
	computeQueue.waitIdle();

	// Create a host visible buffer to retrieve mandelbrot texture
	vdu::Buffer outputBuffer;
	outputBuffer.setMemoryProperty(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	outputBuffer.setUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	outputBuffer.create(&device, resX * resY * outputTexture.getBytesPerPixel());

	// Allocate and write commands to transfer mandelbrot texture to host visible buffer
	vdu::CommandBuffer transferCommands;
	transferCommands.allocate(&device, &cmdPool2);

	transferCommands.begin();
	cmd = transferCommands.getHandle();

	VkBufferImageCopy biCopy{};
	biCopy.imageExtent = VkExtent3D{resX, resY, 1};
	biCopy.imageSubresource = VkImageSubresourceLayers{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};

	vkCmdCopyImageToBuffer(cmd, outputTexture.getHandle(), VK_IMAGE_LAYOUT_GENERAL, outputBuffer.getHandle(), 1, &biCopy);

	transferCommands.end();

	// Submit the transfer commands
	vdu::QueueSubmission transferMandelbrot;
	transferMandelbrot.addCommands(cmd);
	transferMandelbrot.addWait(sem, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
	transferQueue.submit(transferMandelbrot);

	// Wait until the transfer is done and save the texture to a file
	transferQueue.waitIdle();

	unsigned char *dat = (unsigned char *)outputBuffer.getMemory()->map();
	saveBitmapToFile("mandelbrot.bmp", dat, resX, resY);
	outputBuffer.getMemory()->unmap();

	// Cleanup vulkan objects
	vkDestroySampler(device.getHandle(), texSampler, nullptr);
	sem.destroy();
	outputTexture.destroy();
	outputBuffer.destroy();
	pipeline.destroy();
	pipelineLayout.destroy();
	drawCommands.free();
	transferCommands.free();
	cmdPool.destroy();
	if (!transferQueue.sameFamilyAs(computeQueue))
	{
		cmdPool2.destroy();
	}
	descSetLayout.destroy();
	descPool.destroy();
	shader.destroy();
	device.destroy();
	instance.destroy();
}

void saveBitmapToFile(const char *file, unsigned char *bitmap, int width, int height)
{
	// Adapted from:
	// https://stackoverflow.com/a/2654860

	size_t imageSize = 3 * width * height;
	size_t filesize = 54 + imageSize;

	unsigned char *out = new unsigned char[imageSize];
	memset(out, 0, imageSize);

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			int x = i;
			int y = (height - 1) - j;
			out[(x + y * width) * 3 + 2] = (unsigned char)(bitmap[(j * width + i) * 4 + 0]);
			out[(x + y * width) * 3 + 1] = (unsigned char)(bitmap[(j * width + i) * 4 + 1]);
			out[(x + y * width) * 3 + 0] = (unsigned char)(bitmap[(j * width + i) * 4 + 2]);
		}
	}

	unsigned char bmpfileheader[14] = {'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0};
	unsigned char bmpinfoheader[40] = {40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0};
	unsigned char bmppad[3] = {0, 0, 0};

	bmpfileheader[2] = (unsigned char)(filesize);
	bmpfileheader[3] = (unsigned char)(filesize >> 8);
	bmpfileheader[4] = (unsigned char)(filesize >> 16);
	bmpfileheader[5] = (unsigned char)(filesize >> 24);
	bmpinfoheader[4] = (unsigned char)(width);
	bmpinfoheader[5] = (unsigned char)(width >> 8);
	bmpinfoheader[6] = (unsigned char)(width >> 16);
	bmpinfoheader[7] = (unsigned char)(width >> 24);
	bmpinfoheader[8] = (unsigned char)(height);
	bmpinfoheader[9] = (unsigned char)(height >> 8);
	bmpinfoheader[10] = (unsigned char)(height >> 16);
	bmpinfoheader[11] = (unsigned char)(height >> 24);

	FILE *f = fopen(file, "wb");
	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);
	for (int i = 0; i < height; i++)
	{
		fwrite(out + (width * (height - i - 1) * 3), 3, width, f);
		fwrite(bmppad, 1, (4 - (width * 3) % 4) % 4, f);
	}

	delete[] out;
}