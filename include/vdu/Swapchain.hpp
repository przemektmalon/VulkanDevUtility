#pragma once
#include "PCH.hpp"
#include "Framebuffer.hpp"

namespace vdu
{
	class Swapchain
	{
	public:
		Swapchain();

		void create(LogicalDevice* device, VkSurfaceKHR surface);

		void destroy();

		const VkFormat& getFormat() const { return m_imageFormat; }
		const VkExtent2D& getExtent() const { return m_extent; }
		const uint32_t getImageCount() const { return m_imageCount; }
		const VkSwapchainKHR getHandle() const { return m_swapchain; }
		const RenderPass& getRenderPass() const { return m_renderPass; }

		const std::vector<VkImage>& getImages() const { return m_images; }
		const std::vector<VkImageView>& getImageViews() const { return m_imageViews; }
		const std::vector<VkFramebuffer>& getFramebuffers() const { return m_framebuffers; }

	private:

		LogicalDevice * m_logicalDevice;

		VkSurfaceKHR m_surface;
		VkSurfaceFormatKHR m_surfaceFormat;
		VkFormat m_imageFormat;
		VkPresentModeKHR m_presentMode;
		VkExtent2D m_extent;
		uint32_t m_imageCount;

		RenderPass m_renderPass;

		VkSwapchainKHR m_swapchain;

		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_imageViews;
		std::vector<VkFramebuffer> m_framebuffers;
	};
}