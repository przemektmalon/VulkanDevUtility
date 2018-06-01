#include "Shaders.hpp"
#include "Initializers.hpp"

vdu::ShaderModule::ShaderModule() : m_stage(ShaderStage(0)), m_language(ShaderLanguage::UNKNOWN), m_module(0), m_logicalDevice(nullptr)
{
}

void vdu::ShaderModule::create(ShaderStage stage, const std::string& path, LogicalDevice** logicalDevice)
{
	m_stage = stage;
	m_path = path;
	m_logicalDevice = logicalDevice;
	setIntStage();
	load();
}

void vdu::ShaderModule::load()
{
	determineLanguage();

	std::fstream file;

	file.open(m_path, std::ios_base::in | std::ios_base::binary);

	if (!file.is_open())
		DBG_WARNING("Failed to open GLSL shader file: " << m_path);

	file.seekg(0, std::ios_base::end);
	size_t shaderSize = file.tellg();
	file.seekg(0);

	m_glslSource.clear();
	m_spirvSource.clear();
	if (m_language == ShaderLanguage::GLSL)
	{
		m_glslSource.resize(shaderSize);
		file.read((char*)&m_glslSource[0], sizeof(char)*shaderSize);
	}
	else if (m_language == ShaderLanguage::SPV)
	{
		m_spirvSource.resize(shaderSize);
		file.read((char*)&m_spirvSource[0], sizeof(char)*shaderSize);
	}
}

bool vdu::ShaderModule::compile()
{
	if (!m_logicalDevice)
	{
		DBG_SEVERE("Attempting to compile an empty (uncreated) shader");
		return false;
	}

	if (m_language == ShaderLanguage::GLSL)
	{
		shaderc::Compiler c;
		shaderc::CompileOptions o;
		o.SetAutoBindUniforms(true);
		o.AddMacroDefinition(m_stageMacro);
		o.SetLimit(shaderc_limit::shaderc_limit_max_combined_texture_image_units, 1024);
		auto res = c.CompileGlslToSpv(m_glslSource, m_internalStage, m_path.c_str(), o);
		if (res.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			DBG_WARNING("\n" << res.GetErrorMessage() << "\n");
			return false;
		}
		m_spirvSource.assign(res.begin(), res.end());
	}
	if (m_spirvSource.size() == 0) {
		DBG_WARNING("Source missing, cannot compile shader: " << m_path);
		return false;
	}
	auto createInfo = vdu::initializer<VkShaderModuleCreateInfo>(m_spirvSource.size() * sizeof(int), m_spirvSource.data());
	
	if (m_module)
		destroy();

	VDU_VK_CHECK_RESULT(vkCreateShaderModule((*m_logicalDevice)->getHandle(), &createInfo, nullptr, &m_module));

	return true;
}

void vdu::ShaderModule::destroy()
{
	vkDestroyShaderModule((*m_logicalDevice)->getHandle(), m_module, 0);
}

void vdu::ShaderModule::setIntStage()
{
	switch (m_stage)
	{
	case ShaderStage::Vertex:
		m_internalStage = shaderc_shader_kind::shaderc_glsl_vertex_shader; m_stageMacro = "VERTEX"; return;
	case ShaderStage::Fragment:
		m_internalStage = shaderc_shader_kind::shaderc_glsl_fragment_shader; m_stageMacro = "FRAGMENT"; return;
	case ShaderStage::Compute:
		m_internalStage = shaderc_shader_kind::shaderc_glsl_compute_shader; m_stageMacro = "COMPUTE"; return;
	case ShaderStage::Geometry:
		m_internalStage = shaderc_shader_kind::shaderc_glsl_geometry_shader; m_stageMacro = "GEOMETRY"; return;
	case ShaderStage::TessControl:
		m_internalStage = shaderc_shader_kind::shaderc_glsl_tess_control_shader; m_stageMacro = "TESSCONTROL"; return;
	case ShaderStage::TessEval:
		m_internalStage = shaderc_shader_kind::shaderc_glsl_tess_evaluation_shader; m_stageMacro = "TESSEVAL"; return;
	}
}

void vdu::ShaderModule::determineLanguage()
{
	int i = 0;
	while (m_path[i] != '.')
	{
		++i;
		if (i > m_path.length())
			DBG_WARNING("Bad shader file name format: " << m_path);
	}
	std::string extension;
	extension.assign(&m_path[i + 1]);
	if (extension == "glsl" || extension == "GLSL")
		m_language = ShaderLanguage::GLSL;
	else if (extension == "spv" || extension == "SPV")
		m_language = ShaderLanguage::SPV;
	else
	{
		DBG_WARNING("Bad shader file extension: " << m_path << " - '.glsl' and 'spv' supported");
		m_language = ShaderLanguage::UNKNOWN;
	}
}

void vdu::ShaderProgram::create(LogicalDevice * logicalDevice)
{
	m_logicalDevice = logicalDevice;
	reload();
}

void vdu::ShaderProgram::destroy()
{
	for (auto& m : m_modules)
	{
		m.destroy();
	}
}

void vdu::ShaderProgram::addModule(ShaderStage stage, const std::string & path)
{
	m_modules.push_back(ShaderModule());
	m_modules.back().create(stage, path, &m_logicalDevice);
}

void vdu::ShaderProgram::reload()
{
	for (auto& m : m_modules)
	{
		m.load();
	}
}

void vdu::ShaderProgram::compile()
{
	m_shaderStageCreateInfos.clear();
	for (auto& m : m_modules)
	{
		m.compile();
		m_shaderStageCreateInfos.push_back(vdu::initializer<VkPipelineShaderStageCreateInfo>(static_cast<VkShaderStageFlagBits>(m.getStage()), m.getHandle(), "main"));
	}
}

const VkPipelineShaderStageCreateInfo * vdu::ShaderProgram::getShaderStageCreateInfos()
{
	return m_shaderStageCreateInfos.data();
}

int vdu::ShaderProgram::getNumStages()
{
	return m_shaderStageCreateInfos.size();
}