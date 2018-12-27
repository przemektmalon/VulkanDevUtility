#include "PCH.hpp"
#include "RenderPass.hpp"

void vdu::RenderPass::create(vdu::LogicalDevice * device)
{
	m_logicalDevice = device;

	std::vector<VkAttachmentReference> refs;
	std::vector<VkAttachmentDescription> descs;
	int attIndex = 0;
	for (auto i : m_attachmentInfos)
	{
		refs.push_back(i.second->getReference());
		if (m_depthAttachmentInfo)
			if (attIndex == m_depthAttachmentInfo->getReference().attachment)
				descs.push_back(m_depthAttachmentInfo->getDescription());
		descs.push_back(i.second->getDescription());
		++attIndex;
	}
	if (m_depthAttachmentInfo)
		if (attIndex == m_depthAttachmentInfo->getReference().attachment)
			descs.push_back(m_depthAttachmentInfo->getDescription());

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = refs.size();
	subpass.pColorAttachments = refs.data();
	subpass.pDepthStencilAttachment = m_depthAttachmentInfo ? &m_depthAttachmentInfo->getReference() : 0;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = descs.size();
	renderPassInfo.pAttachments = descs.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VDU_VK_CHECK_RESULT(vkCreateRenderPass(m_logicalDevice->getHandle(), &renderPassInfo, nullptr, &m_renderPass), "creating render pass");
}

void vdu::RenderPass::destroy()
{
	vkDestroyRenderPass(m_logicalDevice->getHandle(), m_renderPass, 0);
	m_attachments.clear();
	m_attachmentInfos.clear();
	m_depthAttachment = nullptr;
	m_depthAttachmentInfo = nullptr;
	m_renderPass = 0;
}

vdu::RenderPass::AttachmentInfo* vdu::RenderPass::addColourAttachment(vdu::Texture * texture, std::string name)
{
	m_attachments.insert(std::make_pair(name, texture));
	auto insertion = m_attachmentInfos.insert(std::make_pair(name, new AttachmentInfo()));

	m_attachmentInfos[name]->setFormat(texture->getFormat());
	m_attachmentInfos[name]->setAttachmentIndex(m_attachments.size() - 1 + (m_depthAttachment ? 1 : 0));

	return insertion.first->second;
}

vdu::RenderPass::AttachmentInfo* vdu::RenderPass::addColourAttachment(VkFormat format, std::string name)
{
	m_attachments.insert(std::make_pair(name, nullptr));
	auto insertion = m_attachmentInfos.insert(std::make_pair(name, new AttachmentInfo()));

	m_attachmentInfos[name]->setFormat(format);
	m_attachmentInfos[name]->setAttachmentIndex(m_attachments.size() - 1 + (m_depthAttachment ? 1 : 0));

	return insertion.first->second;
}

vdu::RenderPass::AttachmentInfo* vdu::RenderPass::setDepthAttachment(vdu::Texture * texture)
{
	m_depthAttachment = texture;

	m_depthAttachmentInfo = new AttachmentInfo();

	m_depthAttachmentInfo->setFormat(texture->getFormat());
	m_depthAttachmentInfo->setAttachmentIndex(m_attachments.size());

	return m_depthAttachmentInfo;
}

vdu::RenderPass::AttachmentInfo* vdu::RenderPass::setDepthAttachment(VkFormat format)
{
	m_depthAttachment = (vdu::Texture*)1;

	m_depthAttachmentInfo = new AttachmentInfo();

	m_depthAttachmentInfo->setFormat(format);
	m_depthAttachmentInfo->setAttachmentIndex(m_attachments.size());

	return m_depthAttachmentInfo;
}

vdu::RenderPass::AttachmentInfo::AttachmentInfo()
{
	m_description.format = VK_FORMAT_UNDEFINED;
	m_description.samples = VK_SAMPLE_COUNT_1_BIT;
	m_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	m_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	m_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	m_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	m_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	m_description.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	m_description.flags = 0;

	m_reference.attachment = 0;
	m_reference.layout = VK_IMAGE_LAYOUT_UNDEFINED;
}

void vdu::RenderPass::AttachmentInfo::setFormat(VkFormat format)
{
	m_description.format = format;
}

void vdu::RenderPass::AttachmentInfo::setAttachmentIndex(uint32_t index)
{
	m_reference.attachment = index;
}

void vdu::RenderPass::AttachmentInfo::setInitialLayout(VkImageLayout layout)
{
	m_description.initialLayout = layout;
}

void vdu::RenderPass::AttachmentInfo::setUsageLayout(VkImageLayout layout)
{
	m_reference.layout = layout;
}

void vdu::RenderPass::AttachmentInfo::setFinalLayout(VkImageLayout layout)
{
	m_description.finalLayout = layout;
}

void vdu::RenderPass::AttachmentInfo::setLoadOp(VkAttachmentLoadOp loadOp)
{
	m_description.loadOp = loadOp;
}

void vdu::RenderPass::AttachmentInfo::setStoreOp(VkAttachmentStoreOp storeOp)
{
	m_description.storeOp = storeOp;
}
