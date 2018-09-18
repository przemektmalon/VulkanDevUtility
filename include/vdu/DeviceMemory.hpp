#pragma once
#include "PCH.hpp"
#include "LogicalDevice.hpp"
#include "CommandBuffer.hpp"

namespace vdu
{
	class DeviceMemory
	{
	public:
		DeviceMemory() : m_logicalDevice(nullptr), m_deviceMemory(0), m_deviceSize(0), m_memoryProperties(0) {}

		void allocate(LogicalDevice* logicalDevice, VkDeviceSize size, VkMemoryPropertyFlags memFlags, VkMemoryRequirements memReqs);
		void free();

		void* map() const;
		void* map(VkDeviceSize offset, VkDeviceSize size) const;
		void unmap() const;

		const VkDeviceMemory& getHandle() { return m_deviceMemory; }
		VkDeviceSize getSize() { return m_deviceSize; }

	private:

		LogicalDevice * m_logicalDevice;
		VkDeviceMemory m_deviceMemory;
		VkDeviceSize m_deviceSize;
		VkMemoryPropertyFlags m_memoryProperties;
	};

	class Texture;

	class Buffer
	{
	public:
		Buffer() {}
		Buffer(Buffer& stagingDestination);
		Buffer(Buffer& stagingDestination, VkDeviceSize size);
		Buffer(LogicalDevice* logicalDevice, VkDeviceSize size);
		void create(LogicalDevice* logicalDevice, VkDeviceSize pSize);
		void destroy();

		void addUsingQueueFamily(const QueueFamily* queueFamily);

		void setUsage(VkBufferUsageFlags usage);
		void setMemoryProperty(VkMemoryPropertyFlags memProperty);

		const DeviceMemory* getMemory() { return m_deviceMemory; }
		VkBuffer getHandle() { return m_buffer; }
		
		VkBufferUsageFlags getUsageFlags() { return m_usageFlags; }

		void bindMemory(DeviceMemory* memory);

		void cmdCopyTo(CommandBuffer* cmd, Buffer* dst, VkDeviceSize range = 0, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0);

		void cmdCopyTo(const VkCommandBuffer& cmd, Buffer* dst, VkDeviceSize range = 0, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0);
		void cmdCopyTo(const VkCommandBuffer& cmd, Texture* dst, VkDeviceSize srcOffset = 0, int mipLevel = 0, int baseLayer = 0, int layerCount = 1, VkOffset3D offset = { 0,0,0 }, VkExtent3D extent = { 0,0,0 });

		void createStaging(Buffer& staging); // Creates 'staging' as a buffer to map for 'this'
		void createStaging(Buffer& staging, VkDeviceSize size); // Creates 'staging' as a buffer to map a portion 'this' 
		void createStaging(LogicalDevice* logicalDevice, VkDeviceSize size); // 'this' is created as a mappable staging buffer

	private:

		LogicalDevice * m_logicalDevice;
		DeviceMemory * m_deviceMemory;
		VkBuffer m_buffer;
		VkBufferUsageFlags m_usageFlags;
		VkMemoryPropertyFlags m_memoryProperties;

		std::vector<const QueueFamily*> m_usingQueueFamilies;

	};

	struct TextureCreateInfo
	{
		TextureCreateInfo() : width(0), height(0), depth(1), layers(1), format(VK_FORMAT_UNDEFINED), layout(VK_IMAGE_LAYOUT_UNDEFINED), aspectFlags(VK_IMAGE_ASPECT_COLOR_BIT), usageFlags(VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM), tiling(VK_IMAGE_TILING_OPTIMAL), memoryProperties(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {}
		
		uint32_t width, height, depth, layers;
		VkFormat format;
		VkImageLayout layout;
		VkImageAspectFlags aspectFlags;
		VkImageUsageFlags usageFlags;
		VkImageTiling tiling;
		VkMemoryPropertyFlags memoryProperties;
	};

	class Texture
	{
	public:
		Texture() : m_logicalDevice(nullptr), m_width(0), m_height(0), m_depth(0), m_layers(0), m_image(0), m_imageView(0), m_format(VK_FORMAT_UNDEFINED), m_layout(VK_IMAGE_LAYOUT_UNDEFINED), m_aspectFlags(VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM), m_usageFlags(VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM), m_tiling(VK_IMAGE_TILING_MAX_ENUM){}

		void setProperties(const TextureCreateInfo& ci);
		void create(LogicalDevice* logicalDevice);
		void destroy();

		void cmdGenerateMipMaps(VkCommandBuffer& cmd);
		void bindMemory(DeviceMemory* memory);

		uint32_t getWidth() { return m_width; }
		uint32_t getHeight() { return m_height; }
		uint32_t getDepth() { return m_depth; }
		uint32_t getLayers() { return m_layers; }
		VkImage getHandle() { return m_image; }
		VkImageView getView() { return m_imageView; }
		VkFormat getFormat() { return m_format; }
		VkImageLayout getLayout() { return m_layout; }
		VkImageAspectFlags getAspectFlags() { return m_aspectFlags; }
		VkImageUsageFlags getUsageFlags() { return m_usageFlags; }
		DeviceMemory* getMemory() { return m_deviceMemory; }
		uint32_t getMaxMipLevel() { return m_maxMipLevel; }
		uint32_t getNumMipLevels() { return m_numMipLevels; }

		uint32_t getBytesPerPixel();
		uint32_t getNumComponents();

	protected:

		LogicalDevice * m_logicalDevice;

		uint32_t m_width, m_height, m_depth;
		uint32_t m_layers;
		VkImage m_image;
		VkImageView m_imageView;
		VkFormat m_format;
		VkImageLayout m_layout;
		VkImageAspectFlags m_aspectFlags;
		VkImageUsageFlags m_usageFlags;
		VkImageTiling m_tiling;

		VkMemoryPropertyFlags m_memoryProperties;

		DeviceMemory* m_deviceMemory;
		
		uint32_t m_maxMipLevel;
		uint32_t m_numMipLevels;
	};

}