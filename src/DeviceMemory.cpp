#include "DeviceMemory.hpp"
#include "Initializers.hpp"

void vdu::DeviceMemory::allocate(LogicalDevice * logicalDevice, VkDeviceSize size, VkMemoryPropertyFlags memFlags, VkMemoryRequirements memReqs)
{
	m_logicalDevice = logicalDevice;
	m_memoryProperties = memFlags;
	m_deviceSize = size;

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = m_logicalDevice->getPhysicalDevice()->findMemoryTypeIndex(memReqs.memoryTypeBits, memFlags);

	VDU_VK_CHECK_RESULT(vkAllocateMemory(m_logicalDevice->getHandle(), &allocInfo, nullptr, &m_deviceMemory), "allocating device memory");
}

void vdu::DeviceMemory::free()
{
	vkFreeMemory(m_logicalDevice->getHandle(), m_deviceMemory, nullptr);
}

void * vdu::DeviceMemory::map() const
{
	if (m_memoryProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	{
		VDU_DBG_SEVERE("Attempting to map an unmappable buffer");
		return nullptr;
	}
	void* data;
	VDU_VK_CHECK_RESULT(vkMapMemory(m_logicalDevice->getHandle(), m_deviceMemory, 0, m_deviceSize, 0, &data), "mapping device memory");
	return data;
}

void * vdu::DeviceMemory::map(VkDeviceSize offset, VkDeviceSize size) const
{
	if (m_memoryProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	{
		VDU_DBG_SEVERE("Attempting to map an unmappable buffer");
		return nullptr;
	}
	void* data;
	VDU_VK_CHECK_RESULT(vkMapMemory(m_logicalDevice->getHandle(), m_deviceMemory, offset, size, 0, &data), "mapping device memory");
	return data;
}

void vdu::DeviceMemory::unmap() const
{
	vkUnmapMemory(m_logicalDevice->getHandle(), m_deviceMemory);
}

vdu::Buffer::Buffer(Buffer & stagingDestination)
{
	createStaging(stagingDestination);
}

vdu::Buffer::Buffer(Buffer & stagingDestination, VkDeviceSize size)
{
	createStaging(stagingDestination.m_logicalDevice, size);
}

vdu::Buffer::Buffer(LogicalDevice * logicalDevice, VkDeviceSize size)
{
	create(logicalDevice, size);
}

void vdu::Buffer::create(LogicalDevice * logicalDevice, VkDeviceSize size)
{
	if (m_usageFlags == 0 || m_memoryProperties == 0)
	{
		VDU_DBG_SEVERE("Attempting to create buffer with no usage flags or memory properties");
		return;
	}

	m_logicalDevice = logicalDevice;

	auto bci = vdu::initializer<VkBufferCreateInfo>();
	bci.usage = m_usageFlags;
	bci.size = size;

	std::vector<uint32_t> sharingQueueFamilyIndices;
	if (m_usingQueueFamilies.size() < 2)
	{
		bci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	else
	{
		bci.sharingMode = VK_SHARING_MODE_CONCURRENT;
		bci.queueFamilyIndexCount = m_usingQueueFamilies.size();
		for (auto& qf : m_usingQueueFamilies)
		{
			sharingQueueFamilyIndices.push_back(qf->getIndex());
		}
		bci.pQueueFamilyIndices = sharingQueueFamilyIndices.data();
	}

	VDU_VK_CHECK_RESULT(vkCreateBuffer(m_logicalDevice->getHandle(), &bci, nullptr, &m_buffer), "creating buffer");

	m_deviceMemory = new DeviceMemory();
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_logicalDevice->getHandle(), m_buffer, &memRequirements);
	m_deviceMemory->allocate(m_logicalDevice, size, m_memoryProperties, memRequirements);

	bindMemory(m_deviceMemory);
}

void vdu::Buffer::destroy()
{
	vkDestroyBuffer(m_logicalDevice->getHandle(), m_buffer, nullptr);
	m_deviceMemory->free();
	delete m_deviceMemory;
	m_deviceMemory = 0;
	m_buffer = 0;
}

void vdu::Buffer::addUsingQueueFamily(const QueueFamily * queueFamily)
{
	m_usingQueueFamilies.push_back(queueFamily);
}

void vdu::Buffer::setUsage(VkBufferUsageFlags usage)
{
	m_usageFlags = usage;
}

void vdu::Buffer::setMemoryProperty(VkMemoryPropertyFlags memProperty)
{
	m_memoryProperties = memProperty;
}

void vdu::Buffer::bindMemory(DeviceMemory * memory)
{
	VDU_VK_CHECK_RESULT(vkBindBufferMemory(m_logicalDevice->getHandle(), m_buffer, memory->getHandle() , 0), "binding buffer memory");
}

void vdu::Buffer::cmdCopyTo(CommandBuffer * cmd, Buffer * dst, VkDeviceSize range, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
{
	cmdCopyTo(cmd->getHandle(), dst, range, srcOffset, dstOffset);
}

void vdu::Buffer::cmdCopyTo(CommandBuffer * cmd, Texture * dst, VkDeviceSize srcOffset, int mipLevel, int baseLayer, int layerCount, VkOffset3D offset, VkExtent3D extent)
{
	cmdCopyTo(cmd->getHandle(), dst, srcOffset, mipLevel, baseLayer, layerCount, offset, extent);
}

void vdu::Buffer::cmdCopyTo(const VkCommandBuffer& commandBuffer, Buffer * dst, VkDeviceSize range, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
{
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = srcOffset;
	copyRegion.dstOffset = dstOffset;
	if (range == 0)
		range = m_deviceMemory->getSize();
	copyRegion.size = range;
	vkCmdCopyBuffer(commandBuffer, m_buffer, dst->getHandle(), 1, &copyRegion);
}

void vdu::Buffer::cmdCopyTo(const VkCommandBuffer& commandBuffer, Texture * dst, VkDeviceSize srcOffset, int mipLevel, int baseLayer, int layerCount, VkOffset3D offset, VkExtent3D extent)
{
	if (extent.depth == 0)
		extent.depth = 1;
	if (extent.width == 0)
		extent.width = dst->getWidth();
	if (extent.height == 0)
		extent.height = dst->getHeight();

	VkBufferImageCopy region = {};
	region.bufferOffset = srcOffset;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = mipLevel;
	region.imageSubresource.baseArrayLayer = baseLayer;
	region.imageSubresource.layerCount = layerCount;
	region.imageOffset = offset;
	region.imageExtent = extent;

	vkCmdCopyBufferToImage(commandBuffer, m_buffer, dst->getHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void vdu::Buffer::createStaging(Buffer & staging)
{
	staging.setMemoryProperty(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	staging.setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	staging.create(m_logicalDevice, m_deviceMemory->getSize());
}

void vdu::Buffer::createStaging(Buffer & staging, VkDeviceSize size)
{
	staging.setMemoryProperty(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	staging.setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	staging.create(m_logicalDevice, size);
}

void vdu::Buffer::createStaging(LogicalDevice* logicalDevice, VkDeviceSize size)
{
	setMemoryProperty(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	create(logicalDevice, size);
}

void vdu::Texture::create(LogicalDevice* logicalDevice)
{
	m_logicalDevice = logicalDevice;

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = m_width;
	imageInfo.extent.height = m_height;
	imageInfo.extent.depth = m_depth;
	imageInfo.mipLevels = m_numMipLevels;
	imageInfo.arrayLayers = m_layers;
	imageInfo.format = m_format;
	imageInfo.tiling = m_tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = m_usageFlags;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (m_layers == 6) /// TODO: is this always true ?
		imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	VDU_VK_CHECK_RESULT(vkCreateImage(m_logicalDevice->getHandle(), &imageInfo, nullptr, &m_image), "creating image");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_logicalDevice->getHandle(), m_image, &memRequirements);

	m_deviceMemory = new DeviceMemory;
	m_deviceMemory->allocate(m_logicalDevice, memRequirements.size, m_memoryProperties, memRequirements);
	bindMemory(m_deviceMemory);
	

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = m_image;
	viewInfo.format = m_format;
	viewInfo.subresourceRange.aspectMask = m_aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = m_numMipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = m_layers;
	if (m_layers == 6)
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	else
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

	VDU_VK_CHECK_RESULT(vkCreateImageView(m_logicalDevice->getHandle(), &viewInfo, nullptr, &m_imageView), "creating image view");
}

void vdu::Texture::setProperties(const TextureCreateInfo & ci)
{
	m_width = ci.width; m_height = ci.height; m_depth = ci.depth; m_layers = ci.layers;
	m_format = ci.format;
	m_layout = ci.layout;
	m_aspectFlags = ci.aspectFlags;
	m_usageFlags = ci.usageFlags;
	m_tiling = ci.tiling;
	m_memoryProperties = ci.memoryProperties;
}

void vdu::Texture::destroy()
{
	if (!m_logicalDevice)
		return;
	vkDestroyImageView(m_logicalDevice->getHandle(), m_imageView, nullptr);
	vkDestroyImage(m_logicalDevice->getHandle(), m_image, nullptr);
	m_deviceMemory->free();
}

void vdu::Texture::cmdGenerateMipMaps(const VkCommandBuffer & cmd)
{
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = m_image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = m_width;
	int32_t mipHeight = m_height;

	for (uint32_t i = 1; i < m_numMipLevels; i++) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(cmd,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VkImageBlit blit = {};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth / 2, mipHeight / 2, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(cmd,
			m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmd,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = m_maxMipLevel;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = m_layout;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(cmd,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);
}

void vdu::Texture::cmdGenerateMipMaps(CommandBuffer * cmd)
{
	cmdGenerateMipMaps(cmd->getHandle());
}

void vdu::Texture::bindMemory(DeviceMemory * memory)
{
	VDU_VK_CHECK_RESULT(vkBindImageMemory(m_logicalDevice->getHandle(), m_image, m_deviceMemory->getHandle(), 0), "binding texture memory");
}

uint32_t vdu::Texture::getBytesPerPixel()
{
	switch (m_format)
	{
	case(VK_FORMAT_R4G4_UNORM_PACK8):
	case(VK_FORMAT_R8_UNORM):
	case(VK_FORMAT_R8_SNORM):
	case(VK_FORMAT_R8_USCALED):
	case(VK_FORMAT_R8_SSCALED):
	case(VK_FORMAT_R8_UINT):
	case(VK_FORMAT_R8_SINT):
	case(VK_FORMAT_R8_SRGB):
		return 8;

	case(VK_FORMAT_R4G4B4A4_UNORM_PACK16):
	case(VK_FORMAT_B4G4R4A4_UNORM_PACK16):
	case(VK_FORMAT_R5G5B5A1_UNORM_PACK16):
	case(VK_FORMAT_B5G5R5A1_UNORM_PACK16):
	case(VK_FORMAT_A1R5G5B5_UNORM_PACK16):
	case(VK_FORMAT_R5G6B5_UNORM_PACK16):
	case(VK_FORMAT_B5G6R5_UNORM_PACK16):
	case(VK_FORMAT_R8G8_UNORM):
	case(VK_FORMAT_R8G8_SNORM):
	case(VK_FORMAT_R8G8_USCALED):
	case(VK_FORMAT_R8G8_SSCALED):
	case(VK_FORMAT_R8G8_UINT):
	case(VK_FORMAT_R8G8_SINT):
	case(VK_FORMAT_R8G8_SRGB):
	case(VK_FORMAT_R16_UNORM):
	case(VK_FORMAT_R16_SNORM):
	case(VK_FORMAT_R16_USCALED):
	case(VK_FORMAT_R16_SSCALED):
	case(VK_FORMAT_R16_UINT):
	case(VK_FORMAT_R16_SINT):
	case(VK_FORMAT_R16_SFLOAT):
		return 16;

	case(VK_FORMAT_R8G8B8_UNORM):
	case(VK_FORMAT_R8G8B8_SNORM):
	case(VK_FORMAT_R8G8B8_USCALED):
	case(VK_FORMAT_R8G8B8_SSCALED):
	case(VK_FORMAT_R8G8B8_UINT):
	case(VK_FORMAT_R8G8B8_SINT):
	case(VK_FORMAT_R8G8B8_SRGB):
	case(VK_FORMAT_B8G8R8_UNORM):
	case(VK_FORMAT_B8G8R8_SNORM):
	case(VK_FORMAT_B8G8R8_USCALED):
	case(VK_FORMAT_B8G8R8_SSCALED):
	case(VK_FORMAT_B8G8R8_UINT):
	case(VK_FORMAT_B8G8R8_SINT):
	case(VK_FORMAT_B8G8R8_SRGB):
		return 24;

	case(VK_FORMAT_R8G8B8A8_UNORM):
	case(VK_FORMAT_R8G8B8A8_SNORM):
	case(VK_FORMAT_R8G8B8A8_USCALED):
	case(VK_FORMAT_R8G8B8A8_SSCALED):
	case(VK_FORMAT_R8G8B8A8_UINT):
	case(VK_FORMAT_R8G8B8A8_SINT):
	case(VK_FORMAT_R8G8B8A8_SRGB):
	case(VK_FORMAT_B8G8R8A8_UNORM):
	case(VK_FORMAT_B8G8R8A8_SNORM):
	case(VK_FORMAT_B8G8R8A8_USCALED):
	case(VK_FORMAT_B8G8R8A8_SSCALED):
	case(VK_FORMAT_B8G8R8A8_UINT):
	case(VK_FORMAT_B8G8R8A8_SINT):
	case(VK_FORMAT_B8G8R8A8_SRGB):
	case(VK_FORMAT_A8B8G8R8_UNORM_PACK32):
	case(VK_FORMAT_A8B8G8R8_SNORM_PACK32):
	case(VK_FORMAT_A8B8G8R8_USCALED_PACK32):
	case(VK_FORMAT_A8B8G8R8_SSCALED_PACK32):
	case(VK_FORMAT_A8B8G8R8_UINT_PACK32):
	case(VK_FORMAT_A8B8G8R8_SINT_PACK32):
	case(VK_FORMAT_A8B8G8R8_SRGB_PACK32):
	case(VK_FORMAT_A2R10G10B10_UNORM_PACK32):
	case(VK_FORMAT_A2R10G10B10_SNORM_PACK32):
	case(VK_FORMAT_A2R10G10B10_USCALED_PACK32):
	case(VK_FORMAT_A2R10G10B10_SSCALED_PACK32):
	case(VK_FORMAT_A2R10G10B10_UINT_PACK32):
	case(VK_FORMAT_A2R10G10B10_SINT_PACK32):
	case(VK_FORMAT_A2B10G10R10_UNORM_PACK32):
	case(VK_FORMAT_A2B10G10R10_SNORM_PACK32):
	case(VK_FORMAT_A2B10G10R10_USCALED_PACK32):
	case(VK_FORMAT_A2B10G10R10_SSCALED_PACK32):
	case(VK_FORMAT_A2B10G10R10_UINT_PACK32):
	case(VK_FORMAT_A2B10G10R10_SINT_PACK32):
	case(VK_FORMAT_R16G16_UNORM):
	case(VK_FORMAT_R16G16_SNORM):
	case(VK_FORMAT_R16G16_USCALED):
	case(VK_FORMAT_R16G16_SSCALED):
	case(VK_FORMAT_R16G16_UINT):
	case(VK_FORMAT_R16G16_SINT):
	case(VK_FORMAT_R16G16_SFLOAT):
	case(VK_FORMAT_B10G11R11_UFLOAT_PACK32):
	case(VK_FORMAT_E5B9G9R9_UFLOAT_PACK32):
	case(VK_FORMAT_R32_UINT):
	case(VK_FORMAT_R32_SINT):
	case(VK_FORMAT_R32_SFLOAT):
	case(VK_FORMAT_D32_SFLOAT):
		return 32;

	case(VK_FORMAT_R16G16B16_UNORM):
	case(VK_FORMAT_R16G16B16_SNORM):
	case(VK_FORMAT_R16G16B16_USCALED):
	case(VK_FORMAT_R16G16B16_SSCALED):
	case(VK_FORMAT_R16G16B16_UINT):
	case(VK_FORMAT_R16G16B16_SINT):
	case(VK_FORMAT_R16G16B16_SFLOAT):
		return 48;

	case(VK_FORMAT_R16G16B16A16_UNORM):
	case(VK_FORMAT_R16G16B16A16_SNORM):
	case(VK_FORMAT_R16G16B16A16_USCALED):
	case(VK_FORMAT_R16G16B16A16_SSCALED):
	case(VK_FORMAT_R16G16B16A16_UINT):
	case(VK_FORMAT_R16G16B16A16_SINT):
	case(VK_FORMAT_R16G16B16A16_SFLOAT):
	case(VK_FORMAT_R32G32_UINT):
	case(VK_FORMAT_R32G32_SINT):
	case(VK_FORMAT_R32G32_SFLOAT):
	case(VK_FORMAT_R64_UINT):
	case(VK_FORMAT_R64_SINT):
	case(VK_FORMAT_R64_SFLOAT):
		return 64;

	case(VK_FORMAT_R32G32B32_UINT):
	case(VK_FORMAT_R32G32B32_SINT):
	case(VK_FORMAT_R32G32B32_SFLOAT):
		return 96;

	case(VK_FORMAT_R32G32B32A32_UINT):
	case(VK_FORMAT_R32G32B32A32_SINT):
	case(VK_FORMAT_R32G32B32A32_SFLOAT):
	case(VK_FORMAT_R64G64_UINT):
	case(VK_FORMAT_R64G64_SINT):
	case(VK_FORMAT_R64G64_SFLOAT):
		return 128;

	case(VK_FORMAT_R64G64B64_UINT):
	case(VK_FORMAT_R64G64B64_SINT):
	case(VK_FORMAT_R64G64B64_SFLOAT):
		return 192;

	case(VK_FORMAT_R64G64B64A64_UINT):
	case(VK_FORMAT_R64G64B64A64_SINT):
	case(VK_FORMAT_R64G64B64A64_SFLOAT):
		return 256;

	default:
		return 0;
	};
}

uint32_t vdu::Texture::getNumComponents()
{
	switch (m_format)
	{
	case(VK_FORMAT_R8_UNORM):
	case(VK_FORMAT_R8_SNORM):
	case(VK_FORMAT_R8_USCALED):
	case(VK_FORMAT_R8_SSCALED):
	case(VK_FORMAT_R8_UINT):
	case(VK_FORMAT_R8_SINT):
	case(VK_FORMAT_R8_SRGB):
	case(VK_FORMAT_R16_UNORM):
	case(VK_FORMAT_R16_SNORM):
	case(VK_FORMAT_R16_USCALED):
	case(VK_FORMAT_R16_SSCALED):
	case(VK_FORMAT_R16_UINT):
	case(VK_FORMAT_R16_SINT):
	case(VK_FORMAT_R16_SFLOAT):
	case(VK_FORMAT_R32_UINT):
	case(VK_FORMAT_R32_SINT):
	case(VK_FORMAT_R32_SFLOAT):
	case(VK_FORMAT_D32_SFLOAT):
	case(VK_FORMAT_R64_UINT):
	case(VK_FORMAT_R64_SINT):
	case(VK_FORMAT_R64_SFLOAT):
		return 1;

	case(VK_FORMAT_R4G4_UNORM_PACK8):
	case(VK_FORMAT_R8G8_UNORM):
	case(VK_FORMAT_R8G8_SNORM):
	case(VK_FORMAT_R8G8_USCALED):
	case(VK_FORMAT_R8G8_SSCALED):
	case(VK_FORMAT_R8G8_UINT):
	case(VK_FORMAT_R8G8_SINT):
	case(VK_FORMAT_R8G8_SRGB):
	case(VK_FORMAT_R16G16_UNORM):
	case(VK_FORMAT_R16G16_SNORM):
	case(VK_FORMAT_R16G16_USCALED):
	case(VK_FORMAT_R16G16_SSCALED):
	case(VK_FORMAT_R16G16_UINT):
	case(VK_FORMAT_R16G16_SINT):
	case(VK_FORMAT_R16G16_SFLOAT):
	case(VK_FORMAT_R32G32_UINT):
	case(VK_FORMAT_R32G32_SINT):
	case(VK_FORMAT_R32G32_SFLOAT):
	case(VK_FORMAT_R64G64_UINT):
	case(VK_FORMAT_R64G64_SINT):
	case(VK_FORMAT_R64G64_SFLOAT):
		return 2;

	case(VK_FORMAT_R5G6B5_UNORM_PACK16):
	case(VK_FORMAT_B5G6R5_UNORM_PACK16):
	case(VK_FORMAT_R8G8B8_UNORM):
	case(VK_FORMAT_R8G8B8_SNORM):
	case(VK_FORMAT_R8G8B8_USCALED):
	case(VK_FORMAT_R8G8B8_SSCALED):
	case(VK_FORMAT_R8G8B8_UINT):
	case(VK_FORMAT_R8G8B8_SINT):
	case(VK_FORMAT_R8G8B8_SRGB):
	case(VK_FORMAT_B8G8R8_UNORM):
	case(VK_FORMAT_B8G8R8_SNORM):
	case(VK_FORMAT_B8G8R8_USCALED):
	case(VK_FORMAT_B8G8R8_SSCALED):
	case(VK_FORMAT_B8G8R8_UINT):
	case(VK_FORMAT_B8G8R8_SINT):
	case(VK_FORMAT_B8G8R8_SRGB):
	case(VK_FORMAT_B10G11R11_UFLOAT_PACK32):
	case(VK_FORMAT_R16G16B16_UNORM):
	case(VK_FORMAT_R16G16B16_SNORM):
	case(VK_FORMAT_R16G16B16_USCALED):
	case(VK_FORMAT_R16G16B16_SSCALED):
	case(VK_FORMAT_R16G16B16_UINT):
	case(VK_FORMAT_R16G16B16_SINT):
	case(VK_FORMAT_R16G16B16_SFLOAT):
	case(VK_FORMAT_R32G32B32_UINT):
	case(VK_FORMAT_R32G32B32_SINT):
	case(VK_FORMAT_R32G32B32_SFLOAT):
	case(VK_FORMAT_R64G64B64_UINT):
	case(VK_FORMAT_R64G64B64_SINT):
	case(VK_FORMAT_R64G64B64_SFLOAT):
		return 3;

	case(VK_FORMAT_R4G4B4A4_UNORM_PACK16):
	case(VK_FORMAT_B4G4R4A4_UNORM_PACK16):
	case(VK_FORMAT_R5G5B5A1_UNORM_PACK16):
	case(VK_FORMAT_B5G5R5A1_UNORM_PACK16):
	case(VK_FORMAT_A1R5G5B5_UNORM_PACK16):
	case(VK_FORMAT_R8G8B8A8_UNORM):
	case(VK_FORMAT_R8G8B8A8_SNORM):
	case(VK_FORMAT_R8G8B8A8_USCALED):
	case(VK_FORMAT_R8G8B8A8_SSCALED):
	case(VK_FORMAT_R8G8B8A8_UINT):
	case(VK_FORMAT_R8G8B8A8_SINT):
	case(VK_FORMAT_R8G8B8A8_SRGB):
	case(VK_FORMAT_B8G8R8A8_UNORM):
	case(VK_FORMAT_B8G8R8A8_SNORM):
	case(VK_FORMAT_B8G8R8A8_USCALED):
	case(VK_FORMAT_B8G8R8A8_SSCALED):
	case(VK_FORMAT_B8G8R8A8_UINT):
	case(VK_FORMAT_B8G8R8A8_SINT):
	case(VK_FORMAT_B8G8R8A8_SRGB):
	case(VK_FORMAT_A8B8G8R8_UNORM_PACK32):
	case(VK_FORMAT_A8B8G8R8_SNORM_PACK32):
	case(VK_FORMAT_A8B8G8R8_USCALED_PACK32):
	case(VK_FORMAT_A8B8G8R8_SSCALED_PACK32):
	case(VK_FORMAT_A8B8G8R8_UINT_PACK32):
	case(VK_FORMAT_A8B8G8R8_SINT_PACK32):
	case(VK_FORMAT_A8B8G8R8_SRGB_PACK32):
	case(VK_FORMAT_A2R10G10B10_UNORM_PACK32):
	case(VK_FORMAT_A2R10G10B10_SNORM_PACK32):
	case(VK_FORMAT_A2R10G10B10_USCALED_PACK32):
	case(VK_FORMAT_A2R10G10B10_SSCALED_PACK32):
	case(VK_FORMAT_A2R10G10B10_UINT_PACK32):
	case(VK_FORMAT_A2R10G10B10_SINT_PACK32):
	case(VK_FORMAT_A2B10G10R10_UNORM_PACK32):
	case(VK_FORMAT_A2B10G10R10_SNORM_PACK32):
	case(VK_FORMAT_A2B10G10R10_USCALED_PACK32):
	case(VK_FORMAT_A2B10G10R10_SSCALED_PACK32):
	case(VK_FORMAT_A2B10G10R10_UINT_PACK32):
	case(VK_FORMAT_A2B10G10R10_SINT_PACK32):
	case(VK_FORMAT_R16G16B16A16_UNORM):
	case(VK_FORMAT_R16G16B16A16_SNORM):
	case(VK_FORMAT_R16G16B16A16_USCALED):
	case(VK_FORMAT_R16G16B16A16_SSCALED):
	case(VK_FORMAT_R16G16B16A16_UINT):
	case(VK_FORMAT_R16G16B16A16_SINT):
	case(VK_FORMAT_R16G16B16A16_SFLOAT):
	case(VK_FORMAT_R32G32B32A32_UINT):
	case(VK_FORMAT_R32G32B32A32_SINT):
	case(VK_FORMAT_R32G32B32A32_SFLOAT):
	case(VK_FORMAT_R64G64B64A64_UINT):
	case(VK_FORMAT_R64G64B64A64_SINT):
	case(VK_FORMAT_R64G64B64A64_SFLOAT):
		return 4;

	default:
		return 0;
	};
}
