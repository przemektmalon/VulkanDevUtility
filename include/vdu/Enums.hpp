#pragma once

namespace vdu
{
	typedef uint32_t DescriptorTypeFlags;
	enum DescriptorType
	{
		Sampler = 0,
		CombinedImageSampler = 1,
		SampledImage = 2,
		StorageImage = 3,
		UniformTexelBuffer = 4,
		StorageTexelBuffer = 5,
		UniformBuffer = 6,
		StorageBuffer = 7,
		UniformBufferDynamic = 8,
		StorageBufferDynamic = 9,
		InputAttachment = 10
	};

	typedef uint32_t ShaderStageFlags;
	enum ShaderStage
	{
		Vertex = 0x00000001,
		TessControl = 0x00000002,
		TessEval = 0x00000004,
		Geometry = 0x00000008,
		Fragment = 0x00000010,
		Compute = 0x00000020,
		AllGraphics = 0x0000001F,
		All = 0x7FFFFFFF
	};

	enum class ShaderLanguage
	{
		GLSL, SPV, UNKNOWN
	};
}