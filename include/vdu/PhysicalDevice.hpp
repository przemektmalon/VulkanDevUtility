#pragma once
#include "PCH.hpp"
#include "Instance.hpp"

namespace vdu
{
	// Forward declaration for QueueFamily
	class PhysicalDevice;

	/*
	Wrapper for queue family
	*/
	class QueueFamily
	{
	public:
		QueueFamily(PhysicalDevice* physicalDevice, VkQueueFamilyProperties props, uint32_t familyIndex);

		void queryPresentCapability(VkSurfaceKHR surface);

		bool supportsGraphics() const { return m_supportsGraphics; }
		bool supportsCompute() const { return m_supportsCompute; }
		bool supportsTransfer() const { return m_supportsTransfer; }
		bool supportsPresent() const { return m_supportsPresent; }

		uint32_t getQueueCount() const { return m_familyProperties.queueCount; }
		uint32_t getTimestampValidBits() const { return m_familyProperties.timestampValidBits; }
		VkExtent3D getMinImageTransferGranularity() const { return m_familyProperties.minImageTransferGranularity; }

		uint32_t getIndex() const { return m_familyIndex; }

	private:

		/*
		Fill in properties
		*/
		void queryDetails();

		PhysicalDevice* m_physicalDevice;

		VkQueueFamilyProperties m_familyProperties;

		uint32_t m_familyIndex;

		bool m_supportsGraphics;
		bool m_supportsCompute;
		bool m_supportsTransfer;
		bool m_supportsPresent;
	};

	/*
	Wrapper for a physical device (GPU/CPU/Integrated)
	*/
	class PhysicalDevice
	{
	public:
		PhysicalDevice(VkPhysicalDevice device) : m_physicalDevice(device) { queryDetails(); }

		/*
		Query surface capabilities
		*/
		void querySurfaceCapabilities(VkSurfaceKHR surface);

		VkPhysicalDevice getHandle() const;

		const std::vector<QueueFamily>& getQueueFamilies() const;

		/*
		Functions for finding memory type indices, supported formats from a list of candidates, and an optimal depth format
		*/
		uint32_t findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat findOptimalDepthFormat();

		const VkSurfaceCapabilitiesKHR& getSurfaceCapabilities();
		const std::vector<VkSurfaceFormatKHR>& getSurfaceFormats();
		const std::vector<VkPresentModeKHR>& getPresentModes();

	private:

		/*
		Query and fill in properties and features
		*/
		void queryDetails();

		/*
		Dedicated GPUs are better than CPUs
		*/
		int32_t m_suitabilityScore;

		std::vector<QueueFamily> m_queueFamilies;

		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_deviceProperties;
		VkPhysicalDeviceFeatures m_deviceFeatures;

		VkPhysicalDeviceMemoryProperties m_memoryProperties;

		VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
		std::vector<VkPresentModeKHR> m_presentModes;
	};

	/*
	Return a list of vulkan capable physical devices with their properties fields filled out
	*/
	void enumeratePhysicalDevices(Instance& instance, std::vector<PhysicalDevice>& deviceList);
}