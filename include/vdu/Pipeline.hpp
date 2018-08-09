#pragma once
#include "PCH.hpp"
#include "LogicalDevice.hpp"
#include "RenderPass.hpp"
#include "Swapchain.hpp"
#include "Descriptors.hpp"
#include "Shaders.hpp"

namespace vdu
{

	class VertexInputState
	{
	public:

		void addBinding(uint32_t binding, uint32_t stride, VkVertexInputRate rate = VK_VERTEX_INPUT_RATE_VERTEX);
		void addAttribute(uint32_t binding, uint32_t location, uint32_t offset, VkFormat format);

		void addBinding(VkVertexInputBindingDescription& binding);
		void addAttributes(const std::vector<VkVertexInputAttributeDescription>& attributes);

		const std::vector<VkVertexInputBindingDescription>& getBindings() { return m_bindings; }
		const std::vector<VkVertexInputAttributeDescription>& getAttributes() { return m_attributes; }

	private:

		std::vector<VkVertexInputBindingDescription> m_bindings;
		std::vector<VkVertexInputAttributeDescription> m_attributes;

	};

	class PipelineLayout
	{
	public:

		void create(LogicalDevice* device);

		void destroy();

		void addDescriptorSetLayout(DescriptorSetLayout* layout);
		void addPushConstantRange(VkPushConstantRange range);

		const VkPipelineLayout& getHandle() { return m_layout; }

	private:

		std::vector<DescriptorSetLayout*> m_descriptorSetLayouts;
		std::vector<VkPushConstantRange> m_pushConstantRanges;

		LogicalDevice* m_logicalDevice;

		VkPipelineLayout m_layout;
	};

	class Pipeline
	{
	public:

		void setPipelineLayout(PipelineLayout* layout);
		void setShaderProgram(ShaderProgram* shader);
		

		const VkPipeline& getHandle() { return m_pipeline; }
		const PipelineLayout* getLayout() { return m_layout; }

		void destroy();

	protected:

		VkPipeline m_pipeline;
		PipelineLayout* m_layout;

		DescriptorSet* m_descriptorSet;
		DescriptorSetLayout* m_descriptorSetLayout;

		ShaderProgram* m_shaderProgram;

		LogicalDevice* m_logicalDevice;
	};

	class GraphicsPipeline : public Pipeline
	{
	public:
		GraphicsPipeline();

		void setRenderPass(RenderPass* renderPass);
		void setSwapchain(Swapchain* swapchain);

		void create(LogicalDevice* device);

		void setVertexInputState(VertexInputState* state);

		void setPrimitiveTopology(VkPrimitiveTopology topology);
		void setPrimitiveRestart(VkBool32 enable);

		void addViewport(VkViewport viewport, VkRect2D scissor);

		void setDepthClamp(VkBool32 enable);
		void setRasterizerDiscard(VkBool32 enable);
		void setPolygonMode(VkPolygonMode mode);
		void setLineWidth(float width);
		void setCullMode(VkCullModeFlags mode);
		void setFrontFace(VkFrontFace face);
		void setDepthBias(VkBool32 enable);

		void setSampleShading(VkBool32 enable);
		void setRasterSamples(VkSampleCountFlagBits sampleCount);

		void setAttachmentColorBlendState(std::string attachment, VkPipelineColorBlendAttachmentState blendState);
		void setColorBlendLogicOp(VkBool32 enable, VkLogicOp op = VK_LOGIC_OP_COPY);
		void setColorBlendConstants(float r, float g, float b, float a);

		void setDepthTest(VkBool32 enable);
		void setDepthWrite(VkBool32 enable);
		void setDepthCompare(VkCompareOp op);
		void setDepthBoundsTest(VkBool32 enable);
		void setMinDepthBounds(float min);
		void setMaxDepthBounds(float max);

		void addDynamicState(VkDynamicState state);

	private:

		VertexInputState * m_vertexInputState;

		std::vector<VkViewport> m_viewports;
		std::vector<VkRect2D> m_scissors;
		std::vector<VkDynamicState> m_dynamicState;

		std::map<std::string, VkPipelineColorBlendAttachmentState> m_blendState;
		VkBool32 m_logicOpEnable;
		VkLogicOp m_logicOp;
		std::array<float, 4> m_colorBlendConstants;

		VkPipelineInputAssemblyStateCreateInfo m_assemblyState;
		VkPipelineRasterizationStateCreateInfo m_rasterizerState;
		VkPipelineMultisampleStateCreateInfo m_multisampleState;
		VkPipelineDepthStencilStateCreateInfo m_depthStencilState;

		RenderPass* m_renderPass;
		Swapchain* m_swapchain;
	};

	class ComputePipeline : public Pipeline
	{
	public:

		void create(LogicalDevice* device);

	};
}