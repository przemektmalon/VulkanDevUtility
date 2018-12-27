#pragma once
#include "PCH.hpp"
#include "LogicalDevice.hpp"
#include "DeviceMemory.hpp"

namespace vdu
{
	class RenderPass
	{
	public:
		class AttachmentInfo
		{
		public:
			AttachmentInfo();

			void setFormat(VkFormat format);
			void setAttachmentIndex(uint32_t index);

			void setInitialLayout(VkImageLayout layout);
			void setUsageLayout(VkImageLayout layout);
			void setFinalLayout(VkImageLayout layout);

			void setLoadOp(VkAttachmentLoadOp loadOp);
			void setStoreOp(VkAttachmentStoreOp storeOp);

			const VkAttachmentDescription& getDescription() { return m_description; }
			const VkAttachmentReference& getReference() { return m_reference; }

		protected:
			VkAttachmentDescription m_description;
			VkAttachmentReference m_reference;
		};

		void create(vdu::LogicalDevice* device);

		void destroy();

		const VkRenderPass& getHandle() const { return m_renderPass; }

		AttachmentInfo* addColourAttachment(vdu::Texture* texture, std::string name);
		AttachmentInfo* addColourAttachment(VkFormat format, std::string name);
		AttachmentInfo* setDepthAttachment(vdu::Texture* texture);
		AttachmentInfo* setDepthAttachment(VkFormat format);

		const std::unordered_map<std::string, vdu::Texture*>& getAttachments() const { return m_attachments; }

	private:

		std::unordered_map<std::string, vdu::Texture*> m_attachments;
		std::unordered_map<std::string, AttachmentInfo*> m_attachmentInfos;

		vdu::Texture* m_depthAttachment = nullptr;
		AttachmentInfo* m_depthAttachmentInfo = nullptr;

		VkRenderPass m_renderPass = 0;

		vdu::LogicalDevice* m_logicalDevice = nullptr;
	};
}