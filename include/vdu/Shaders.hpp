#pragma once
#include "PCH.hpp"
#include "Enums.hpp"
#include "shaderc\shaderc.hpp"
#include "LogicalDevice.hpp"

namespace vdu
{
	class ShaderModule
	{	
	public:
		ShaderModule();
		void create(ShaderStage stage, const std::string& path, LogicalDevice** logicalDevice);

		VkShaderModule getHandle() { return m_module; }
		ShaderStage getStage() { return m_stage; }

		void load();
		bool compile();
		void destroy();

	private:
		ShaderStage m_stage;
		shaderc_shader_kind m_internalStage;
		std::string m_stageMacro;

		ShaderLanguage m_language;

		std::string m_path;
		std::string m_glslSource;
		std::vector<uint32_t> m_spirvSource;

		VkShaderModule m_module;

		std::string m_infoLog;
		std::string m_debugLog;

		LogicalDevice** m_logicalDevice;

		void setIntStage();
		void determineLanguage();
	};

	class ShaderProgram
	{
	public:
		ShaderProgram() : m_logicalDevice(nullptr) {}

		void create(LogicalDevice* logicalDevice);

		void destroy();

		void addModule(ShaderStage stage, const std::string& path);

		void reload();
		void compile();

		const VkPipelineShaderStageCreateInfo * getShaderStageCreateInfos();

		int getNumStages();

	private:

		std::vector<ShaderModule> m_modules;
		LogicalDevice* m_logicalDevice;

		std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos;

	};
}