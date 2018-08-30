#include "PCH.hpp"
#include "Framebuffer.hpp"

void vdu::Framebuffer::addAttachment(vdu::Texture * att, std::string name)
{
	m_attachments.push_back(att);
	if (name.length() == 0)
		m_attachmentNames.push_back("Attachment " + std::to_string(m_attachments.size()));
	else
		m_attachmentNames.push_back(name);
}

void vdu::Framebuffer::create(vdu::LogicalDevice * device, vdu::RenderPass * renderPass)
{
	m_logicalDevice = device;
	m_renderPass = renderPass;

	std::vector<VkImageView> attachmentViews;

	for (auto att : m_attachments)
	{
		attachmentViews.push_back(att->getView());
	}

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = m_renderPass->getHandle();;
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachmentViews.size());
	framebufferInfo.pAttachments = attachmentViews.data();
	framebufferInfo.width = m_attachments[0]->getWidth();
	framebufferInfo.height = m_attachments[0]->getHeight();
	framebufferInfo.layers = 1;

	VDU_VK_CHECK_RESULT(vkCreateFramebuffer(m_logicalDevice->getHandle(), &framebufferInfo, nullptr, &m_framebuffer));
}

void vdu::Framebuffer::destroy()
{
	VDU_VK_VALIDATE(vkDestroyFramebuffer(m_logicalDevice->getHandle(), m_framebuffer, 0));
	m_renderPass = nullptr;
	m_attachments.clear();
	m_attachmentNames.clear();
	m_framebuffer = 0;
}
