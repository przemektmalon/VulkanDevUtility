#include "PCH.hpp"
#include "Pipeline.hpp"

void vdu::Pipeline::setPipelineLayout(PipelineLayout * layout)
{
	m_layout = layout;
}

void vdu::Pipeline::setShaderProgram(vdu::ShaderProgram * shader)
{
	m_shaderProgram = shader;
}

void vdu::Pipeline::destroy()
{
	vkDestroyPipeline(m_logicalDevice->getHandle(), m_pipeline, 0);
}

void vdu::GraphicsPipeline::setRenderPass(RenderPass * renderPass)
{
	m_renderPass = renderPass;
}

void vdu::ComputePipeline::create(vdu::LogicalDevice * device)
{
	m_logicalDevice = device;

	VkComputePipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.stage = m_shaderProgram->getShaderStageCreateInfos()[0];
	pipelineInfo.layout = m_layout->getHandle();

	VDU_VK_CHECK_RESULT(vkCreateComputePipelines(m_logicalDevice->getHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline), "creating compute pipeline");
}

void vdu::GraphicsPipeline::setSwapchain(Swapchain * swapchain)
{
	m_swapchain = swapchain;
	m_renderPass = const_cast<RenderPass*>(&swapchain->getRenderPass());
}

void vdu::GraphicsPipeline::create(vdu::LogicalDevice * device)
{
	m_logicalDevice = device;

	// For submitting vertex layout info
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = m_vertexInputState->getBindings().size();
	vertexInputInfo.vertexAttributeDescriptionCount = m_vertexInputState->getAttributes().size();
	vertexInputInfo.pVertexBindingDescriptions = m_vertexInputState->getBindings().data();
	vertexInputInfo.pVertexAttributeDescriptions = m_vertexInputState->getAttributes().data();

	// Submit info for viewport(s)
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = m_viewports.size();
	viewportState.pViewports = m_viewports.data();
	viewportState.scissorCount = m_scissors.size();
	viewportState.pScissors = m_scissors.data();

	// Color blending info
	std::vector<VkPipelineColorBlendAttachmentState> blendState;

	for (auto att : m_renderPass->getAttachments())
	{
		auto find = m_blendState.find(att.first);
		if (find == m_blendState.end())
		{
			VkPipelineColorBlendAttachmentState blend = {};
			blend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			blend.blendEnable = VK_FALSE;
			blendState.push_back(blend);
		}
		else
		{
			blendState.push_back(find->second);
		}
	}

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = m_logicOpEnable;
	colorBlending.logicOp = m_logicOp;
	colorBlending.attachmentCount = m_renderPass->getAttachments().size();
	colorBlending.pAttachments = blendState.data();
	colorBlending.blendConstants[0] = m_colorBlendConstants[0];
	colorBlending.blendConstants[1] = m_colorBlendConstants[1];
	colorBlending.blendConstants[2] = m_colorBlendConstants[2];
	colorBlending.blendConstants[3] = m_colorBlendConstants[3];

	VkPipelineDynamicStateCreateInfo dsci = {};
	dsci.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dsci.dynamicStateCount = m_dynamicState.size();
	dsci.pDynamicStates = m_dynamicState.data();

	// Collate all the data necessary to create pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = m_shaderProgram->getNumStages();
	pipelineInfo.pStages = m_shaderProgram->getShaderStageCreateInfos();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &m_assemblyState;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &m_rasterizerState;
	pipelineInfo.pMultisampleState = &m_multisampleState;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDepthStencilState = &m_depthStencilState;
	pipelineInfo.layout = m_layout->getHandle();
	pipelineInfo.renderPass = m_renderPass->getHandle();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.flags = 0;
	if (m_dynamicState.size() > 0)
		pipelineInfo.pDynamicState = &dsci;

	VDU_VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_logicalDevice->getHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline), "creating graphics pipeline");
}

void vdu::GraphicsPipeline::destroy()
{
	Pipeline::destroy();
	m_vertexInputState = nullptr;
	m_viewports.clear();
	m_scissors.clear();
	m_dynamicState.clear();
	m_blendState.clear();
	
	GraphicsPipeline();
}

void vdu::PipelineLayout::create(LogicalDevice * device)
{
	m_logicalDevice = device;

	std::vector<VkDescriptorSetLayout> layoutHandles;

	for (auto layout : m_descriptorSetLayouts)
	{
		layoutHandles.push_back(layout->getHandle());
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = layoutHandles.size();
	pipelineLayoutInfo.pSetLayouts = layoutHandles.data();
	pipelineLayoutInfo.pushConstantRangeCount = m_pushConstantRanges.size();
	pipelineLayoutInfo.pPushConstantRanges = reinterpret_cast<VkPushConstantRange*>(m_pushConstantRanges.data());

	VDU_VK_CHECK_RESULT(vkCreatePipelineLayout(m_logicalDevice->getHandle(), &pipelineLayoutInfo, nullptr, &m_layout), "creating pipeline layout");
}

void vdu::PipelineLayout::destroy()
{
	vkDestroyPipelineLayout(m_logicalDevice->getHandle(), m_layout, 0);
	m_descriptorSetLayouts.clear();
	m_pushConstantRanges.clear();
	m_layout = 0;
}

void vdu::PipelineLayout::addDescriptorSetLayout(DescriptorSetLayout * layout)
{
	m_descriptorSetLayouts.push_back(layout);
}

void vdu::PipelineLayout::addPushConstantRange(VkPushConstantRange range)
{
	m_pushConstantRanges.push_back({ range.stageFlags, range.offset, range.size });
}

void vdu::PipelineLayout::addPushConstantRange(vdu::PushConstantRange range)
{
	m_pushConstantRanges.push_back(range);
}

void vdu::VertexInputState::addBinding(uint32_t binding, uint32_t stride, VkVertexInputRate rate)
{
	m_bindings.push_back({ binding, stride, rate });
}

void vdu::VertexInputState::addAttribute(uint32_t binding, uint32_t location, uint32_t offset, VkFormat format)
{
	m_attributes.push_back({ location, binding, format, offset });
}

void vdu::VertexInputState::addBinding(VkVertexInputBindingDescription & binding)
{
	m_bindings.push_back(binding);
}

void vdu::VertexInputState::addAttributes(const std::vector<VkVertexInputAttributeDescription>& attributes)
{
	for (auto att : attributes)
	{
		m_attributes.push_back(att);
	}
}

vdu::GraphicsPipeline::GraphicsPipeline() : m_rasterizerState({}), m_multisampleState({}), m_assemblyState({}), m_depthStencilState({}), m_vertexInputState(nullptr), m_logicOpEnable(VK_FALSE), m_logicOp(VK_LOGIC_OP_COPY), m_renderPass(nullptr), m_swapchain(nullptr)
{
	m_rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_rasterizerState.depthClampEnable = VK_FALSE;
	m_rasterizerState.rasterizerDiscardEnable = VK_FALSE;
	m_rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;
	m_rasterizerState.lineWidth = 1.0f;
	m_rasterizerState.cullMode = VK_CULL_MODE_NONE;
	m_rasterizerState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	m_rasterizerState.depthBiasEnable = VK_FALSE;

	m_multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_multisampleState.sampleShadingEnable = VK_FALSE;
	m_multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	m_assemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_assemblyState.primitiveRestartEnable = VK_FALSE;
	m_assemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	m_depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_depthStencilState.depthTestEnable = VK_TRUE;
	m_depthStencilState.depthWriteEnable = VK_TRUE;
	m_depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
	m_depthStencilState.depthBoundsTestEnable = VK_FALSE;
	m_depthStencilState.minDepthBounds = 0.0f;
	m_depthStencilState.maxDepthBounds = 1000.0f;

	m_colorBlendConstants[0] = 0.f;
	m_colorBlendConstants[1] = 0.f;
	m_colorBlendConstants[2] = 0.f;
	m_colorBlendConstants[3] = 0.f;
}

void vdu::GraphicsPipeline::setVertexInputState(VertexInputState * state)
{
	m_vertexInputState = state;
}

void vdu::GraphicsPipeline::setPrimitiveTopology(VkPrimitiveTopology topology)
{
	m_assemblyState.topology = topology;
}

void vdu::GraphicsPipeline::setPrimitiveRestart(VkBool32 enable)
{
	m_assemblyState.primitiveRestartEnable = enable;
}

void vdu::GraphicsPipeline::addViewport(VkViewport viewport, VkRect2D scissor)
{
	m_viewports.push_back(viewport);
	m_scissors.push_back(scissor);
}

void vdu::GraphicsPipeline::setDepthClamp(VkBool32 enable)
{
	m_rasterizerState.depthClampEnable = enable;
}

void vdu::GraphicsPipeline::setRasterizerDiscard(VkBool32 enable)
{
	m_rasterizerState.rasterizerDiscardEnable = enable;
}

void vdu::GraphicsPipeline::setPolygonMode(VkPolygonMode mode)
{
	m_rasterizerState.polygonMode = mode;
}

void vdu::GraphicsPipeline::setLineWidth(float width)
{
	m_rasterizerState.lineWidth = width;
}

void vdu::GraphicsPipeline::setCullMode(VkCullModeFlags mode)
{
	m_rasterizerState.cullMode = mode;
}

void vdu::GraphicsPipeline::setFrontFace(VkFrontFace face)
{
	m_rasterizerState.frontFace = face;
}

void vdu::GraphicsPipeline::setDepthBias(VkBool32 enable)
{
	m_rasterizerState.depthBiasEnable = enable;
}

void vdu::GraphicsPipeline::setSampleShading(VkBool32 enable)
{
	m_multisampleState.sampleShadingEnable = enable;
}

void vdu::GraphicsPipeline::setRasterSamples(VkSampleCountFlagBits sampleCount)
{
	m_multisampleState.rasterizationSamples = sampleCount;
}

void vdu::GraphicsPipeline::setAttachmentColorBlendState(std::string attachment, VkPipelineColorBlendAttachmentState blendState)
{
	m_blendState[attachment] = blendState;
}

void vdu::GraphicsPipeline::setColorBlendLogicOp(VkBool32 enable, VkLogicOp op)
{
	m_logicOpEnable = enable;
	m_logicOp = op;
}

void vdu::GraphicsPipeline::setColorBlendConstants(float r, float g, float b, float a)
{
	m_colorBlendConstants[0] = r;
	m_colorBlendConstants[1] = g;
	m_colorBlendConstants[2] = b;
	m_colorBlendConstants[3] = a;
}

void vdu::GraphicsPipeline::setDepthTest(VkBool32 enable)
{
	m_depthStencilState.depthTestEnable = enable;
}

void vdu::GraphicsPipeline::setDepthWrite(VkBool32 enable)
{
	m_depthStencilState.depthWriteEnable = enable;
}

void vdu::GraphicsPipeline::setDepthCompare(VkCompareOp op)
{
	m_depthStencilState.depthCompareOp = op;
}

void vdu::GraphicsPipeline::setDepthBoundsTest(VkBool32 enable)
{
	m_depthStencilState.depthBoundsTestEnable = enable;
}

void vdu::GraphicsPipeline::setMinDepthBounds(float min)
{
	m_depthStencilState.minDepthBounds = min;
}

void vdu::GraphicsPipeline::setMaxDepthBounds(float max)
{
	m_depthStencilState.maxDepthBounds = max;
}

void vdu::GraphicsPipeline::addDynamicState(VkDynamicState state)
{
	m_dynamicState.push_back(state);
}
