
#include "SimpleShader.h"

#include <fstream>
#include <d3dcompiler.h>
#include "Utils.h"


ShaderStage::ShaderStage(std::string shaderEntryPointFuncName, std::string compileTarget)
	: m_shaderEntryPointFuncName(shaderEntryPointFuncName)
	, m_compileTarget(compileTarget)
{
}

ShaderStage::~ShaderStage()
{
	if (m_compiledShader != nullptr)
	{
		m_compiledShader->Release();
	}
}

bool ShaderStage::Compile(const wchar_t* compiledShaderPath)
{
	if (compiledShaderPath != nullptr &&
		!m_shaderEntryPointFuncName.empty() &&
		!m_compileTarget.empty())
	{
		ID3DBlob* errorCodes = nullptr;
		unsigned int compileFlags = 0;
#if defined(DEBUG) && defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		HRESULT hr = D3DCompileFromFile
		(	
			compiledShaderPath,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			m_shaderEntryPointFuncName.c_str(),
			m_compileTarget.c_str(),
			compileFlags,
			0,
			&m_compiledShader,
			&errorCodes
		);
		
		if (SUCCEEDED(hr))
		{
			return true;
		}

		REPORT(EReportType::REPORT_TYPE_WARN, "Shader compile failed.");
		if (errorCodes != nullptr)
		{
			errorCodes->Release();
		}
	}
	return false;
}

bool ShaderStage::LoadCompiledShader(const wchar_t* compiledShaderPath)
{
	if (compiledShaderPath != nullptr)
	{
		std::ifstream fin(compiledShaderPath, std::ios::binary);

		if (fin.is_open())
		{
			fin.seekg(0, std::ios_base::end);
			std::wifstream::pos_type compiledShaderSize = (int)fin.tellg();
			fin.seekg(0, std::ios_base::beg);

			D3DCreateBlob((SIZE_T)compiledShaderSize, &m_compiledShader);

			fin.read((char*)m_compiledShader->GetBufferPointer(), compiledShaderSize);
			fin.close();

			return true;
		}
		fin.close();
	}
	return false;
}

SimpleShader::SimpleShader(ShaderStageStates shaderPassStates)
{
	if (shaderPassStates & static_cast<uint8_t>(EShaderStageType::VERTEX_SHADER))
	{
		m_shaderPasses.insert(std::make_pair(EShaderStageType::VERTEX_SHADER, new ShaderStage("VS", "vs_5_1")));
	}
	if (shaderPassStates & static_cast<uint8_t>(EShaderStageType::HULL_SHADER))
	{
		m_shaderPasses.insert(std::make_pair(EShaderStageType::HULL_SHADER, new ShaderStage("HS", "hs_5_1")));
	}
	if (shaderPassStates & static_cast<uint8_t>(EShaderStageType::DOMAIN_SHADER))
	{
		m_shaderPasses.insert(std::make_pair(EShaderStageType::DOMAIN_SHADER, new ShaderStage("DS", "ds_5_1")));
	}
	if (shaderPassStates & static_cast<uint8_t>(EShaderStageType::GEOMETRY_SHADER))
	{
		m_shaderPasses.insert(std::make_pair(EShaderStageType::GEOMETRY_SHADER, new ShaderStage("GS", "gs_5_1")));
	}
	if (shaderPassStates & static_cast<uint8_t>(EShaderStageType::AMPLIFICATION_SHADER))
	{
		m_shaderPasses.insert(std::make_pair(EShaderStageType::AMPLIFICATION_SHADER, new ShaderStage("AS", "as_6_5")));
	}
	if (shaderPassStates & static_cast<uint8_t>(EShaderStageType::MESH_SHADER))
	{
		m_shaderPasses.insert(std::make_pair(EShaderStageType::MESH_SHADER, new ShaderStage("MS", "ms_6_5")));
	}
	if (shaderPassStates & static_cast<uint8_t>(EShaderStageType::PIXEL_SHADER))
	{
		m_shaderPasses.insert(std::make_pair(EShaderStageType::PIXEL_SHADER, new ShaderStage("PS", "ps_5_1")));
	}
	if (shaderPassStates & static_cast<uint8_t>(EShaderStageType::COMPUTE_SHADER))
	{
		m_shaderPasses.insert(std::make_pair(EShaderStageType::COMPUTE_SHADER, new ShaderStage("CS", "cs_6_5")));
	}
}

SimpleShader::~SimpleShader()
{
	for (auto& cur : m_shaderPasses)
	{
		if (cur.second)
		{
			delete cur.second;
		}
	}
	m_shaderPasses.clear();
}

bool SimpleShader::Compile(const wchar_t* shaderCodePath)
{
	if (!m_isCompiled && shaderCodePath != nullptr)
	{
		for (auto cur : m_shaderPasses)
		{
			if (cur.second != nullptr)
			{
				if (!cur.second->Compile(shaderCodePath))
				{
					REPORT(EReportType::REPORT_TYPE_WARN, "Shader compile failed");
					return false;
				}
			}
		}
		m_isCompiled = true;
		return true;
	}
	return false;
}

bool SimpleShader::LoadCompiledShader(EShaderStageType shaderPassType, const wchar_t* compiledShaderCodePath)
{
	if (!m_isCompiled)
	{
		auto iterFind = m_shaderPasses.find(shaderPassType);
		if (iterFind != m_shaderPasses.end())
		{
			if (!iterFind->second->LoadCompiledShader(compiledShaderCodePath))
			{
				REPORT(EReportType::REPORT_TYPE_WARN, "Compiled shader load failed.");
				return false;
			}
			return true;
		}
		else
		{
			REPORT(EReportType::REPORT_TYPE_WARN, "Shader stage find failed.");
			return false;
		}
	}
	REPORT(EReportType::REPORT_TYPE_WARN, "The shader has already been compiled.");
	return false;
}

ID3DBlob* SimpleShader::GetShader(EShaderStageType shaderPassType)
{
	auto iterFind = m_shaderPasses.find(shaderPassType);
	if (iterFind != m_shaderPasses.end())
	{
		if (iterFind->second != nullptr)
		{
			return iterFind->second->GetShader();
		}
	}
	return nullptr;
}