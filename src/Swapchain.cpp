#include "PCH.hpp"
#include "Swapchain.hpp"
#include "PhysicalDevice.hpp"

vdu::Swapchain::Swapchain()
{
}

void vdu::Swapchain::create(LogicalDevice * device, VkSurfaceKHR surface)
{
	m_surface = surface;
	m_logicalDevice = device;

	const auto physicalDevice = device->getPhysicalDevice();

	auto surfaceCapabilities = physicalDevice->getSurfaceCapabilities();
	auto surfaceFormats = physicalDevice->getSurfaceFormats();
	auto presentModes = physicalDevice->getPresentModes();


	m_presentMode = VK_PRESENT_MODE_FIFO_KHR;

	if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		m_surfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}
	else
	{
		bool foundSuitable = false;
		for (const auto& availableFormat : surfaceFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				m_surfaceFormat = availableFormat;
				foundSuitable = true;
			}
		}
		if (!foundSuitable)
			m_surfaceFormat = surfaceFormats[0];
	}

	for (const auto& availablePresentMode : presentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			m_presentMode = availablePresentMode;
			break;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			m_presentMode = availablePresentMode;
		}
	}

	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		m_extent = surfaceCapabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max() };

		actualExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, actualExtent.height));

		m_extent = actualExtent;
	}

	m_imageCount = surfaceCapabilities.minImageCount + 1;
	if (surfaceCapabilities.maxImageCount > 0 && m_imageCount > surfaceCapabilities.maxImageCount) {
		m_imageCount = surfaceCapabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;

	createInfo.minImageCount = m_imageCount;
	createInfo.imageFormat = m_surfaceFormat.format;
	createInfo.imageColorSpace = m_surfaceFormat.colorSpace;
	createInfo.imageExtent = m_extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	/// TODO: support several queue families
	// If we have several queue families then we will need to look into sharing modes
	/*
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily) {
	createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	createInfo.queueFamilyIndexCount = 2;
	createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	*/

	createInfo.preTransform = surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = m_presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VDU_VK_CHECK_RESULT(vkCreateSwapchainKHR(m_logicalDevice->getHandle(), &createInfo, nullptr, &m_swapchain));

	VDU_VK_CHECK_RESULT(vkGetSwapchainImagesKHR(m_logicalDevice->getHandle(), m_swapchain, &m_imageCount, nullptr));
	m_images.resize(m_imageCount);
	VDU_VK_CHECK_RESULT(vkGetSwapchainImagesKHR(m_logicalDevice->getHandle(), m_swapchain, &m_imageCount, m_images.data()));

	m_imageFormat = m_surfaceFormat.format;

	m_imageViews.resize(m_imageCount);

	for (uint32_t i = 0; i < m_imageCount; i++) 
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_images[i];
		viewInfo.format = m_imageFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

		VDU_VK_CHECK_RESULT(vkCreateImageView(m_logicalDevice->getHandle(), &viewInfo, nullptr, &m_imageViews[i]));
	}

	auto screenInfo = m_renderPass.addColourAttachment(m_imageFormat, "screen");
	screenInfo->setInitialLayout(VK_IMAGE_LAYOUT_UNDEFINED);
	screenInfo->setFinalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	screenInfo->setUsageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	m_renderPass.create(m_logicalDevice);

	m_framebuffers.resize(m_imageCount);

	for (size_t i = 0; i < m_imageViews.size(); i++) {

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderPass.getHandle();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &m_imageViews[i];
		framebufferInfo.width = m_extent.width;
		framebufferInfo.height = m_extent.height;
		framebufferInfo.layers = 1;

		VDU_VK_CHECK_RESULT(vkCreateFramebuffer(m_logicalDevice->getHandle(), &framebufferInfo, nullptr, &m_framebuffers[i]));
	}
}

void vdu::Swapchain::destroy()
{
	for (int i = 0; i < m_images.size(); ++i)
	{
		// Gives error, maybe because these images are created from the swap chain by KHR extension
		//vkDestroyImage(device, swapChainImages[i], 0);
		vkDestroyImageView(m_logicalDevice->getHandle(), m_imageViews[i], 0);
		m_imageViews[i] = nullptr;
	}

	for (auto fb : m_framebuffers)
		vkDestroyFramebuffer(m_logicalDevice->getHandle(), fb, 0);

	m_framebuffers.clear();

	vkDestroySwapchainKHR(m_logicalDevice->getHandle(), m_swapchain, 0);

	m_swapchain = nullptr;

	m_renderPass.destroy();
}

const VkResult vdu::Swapchain::acquireNextImage(uint32_t & imageIndex, VkSemaphore signal, const Fence& fence, uint64_t timeout)
{
	return vkAcquireNextImageKHR(m_logicalDevice->getHandle(), m_swapchain, timeout, signal, fence.getHandle(), &imageIndex);
}
