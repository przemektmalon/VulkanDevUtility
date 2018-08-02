#pragma once
#include "PCH.hpp"
#include "LogicalDevice.hpp"
#include "DeviceMemory.hpp"

namespace vdu
{
	class RenderPass
	{
	public:
		RenderPass();

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

		const VkRenderPass& getHandle() { return m_renderPass; }

		AttachmentInfo* addColourAttachment(vdu::Texture* texture, std::string name);
		AttachmentInfo* setDepthAttachment(vdu::Texture* texture);

	private:

		std::map<std::string, vdu::Texture*> m_attachments;
		std::map<std::string, AttachmentInfo*> m_attachmentInfos;

		vdu::Texture* m_depthAttachment;
		AttachmentInfo* m_depthAttachmentInfo;

		VkRenderPass m_renderPass;

		vdu::LogicalDevice* m_logicalDevice;
	};
}