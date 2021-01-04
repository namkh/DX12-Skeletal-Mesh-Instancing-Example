#pragma once

#include <string>
#include <map>

#include "Defaults.h"

class ShaderStage
{
public:
	ShaderStage(std::string shaderEntryPointFuncName = "", std::string compileTarget = "");
	~ShaderStage();

	bool Compile(const wchar_t* compiledShaderPath);
	bool LoadCompiledShader(const wchar_t* compiledShaderPath);

	ID3DBlob* GetShader() { return m_compiledShader; }

private:
	std::string m_shaderEntryPointFuncName = "";
	std::string m_compileTarget = "";

	ID3DBlob* m_compiledShader = nullptr;
};

enum class EShaderStageType
{
	VERTEX_SHADER = 1,
	HULL_SHADER = 1 << 1,
	DOMAIN_SHADER = 1 << 2,
	GEOMETRY_SHADER = 1 << 3,
	MESH_SHADER = 1 << 4,
	AMPLIFICATION_SHADER = 1 << 5,
	PIXEL_SHADER = 1 << 6,
	COMPUTE_SHADER = 1 << 7,
};

class SimpleShader
{
public:
	typedef uint8_t ShaderStageStates;
	static const short NUM_SHADER_PASSES = 5;

public:
	SimpleShader(ShaderStageStates shaderPassStates);
	~SimpleShader();

	bool Compile(const wchar_t* shaderCodePath);
	bool LoadCompiledShader(EShaderStageType shaderPassType, const wchar_t* compiledShaderCodePath);
	ID3DBlob* GetShader(EShaderStageType shaderPassType);
	bool HasShaderPass(EShaderStageType shaderPassType) { return (m_shaderPipelineState & static_cast<uint8_t>(shaderPassType)) != 0; }

private:

	ShaderStageStates m_shaderPipelineState = 0;
	bool m_isCompiled = false;
	std::wstring m_shaderFilePath;

	std::map<EShaderStageType, ShaderStage*> m_shaderPasses;
};