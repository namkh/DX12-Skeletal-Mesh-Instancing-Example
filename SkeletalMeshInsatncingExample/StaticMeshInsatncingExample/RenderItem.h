#pragma once

#include "Defaults.h"
#include "SimpleShader.h"
#include "RenderObject.h"
#include "AnimationUpdator.h"

#include <map>

struct GlobalConstants
{
	XMMATRIX MatViewProj = {};
	XMFLOAT3 LightDir = XMFLOAT3(0.0f, -1.0f, 0.4f);
	float Padding0 = 0.0f;
	XMFLOAT4 Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	XMFLOAT3 CamPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float Padding1 = 0.0f;
};

struct InstanceConstants
{
	XMMATRIX	WorldMat = {};
	uint32_t	MaterialID = 0;
	uint32_t	SkeletonOffset = 0;
	float		Padding0 = 0;
	float		Padding1 = 0;
};

struct MaterialConstants
{
	int DiffuseTexIndex = -1;
	float Padding0 = 0;
	float Padding1 = 0;
	float Padding2 = 0;
};

class RenderItem
{
public:
	virtual ~RenderItem() {}
public:
	virtual bool Initialize(RenderObject* renderObj, 
							StructuredBuffer<GlobalConstants>* globalConstantBuffer,
							uint32_t frameCount = 2);
	virtual void UpdateResources(CommandBuffer* cmdBuf);
	virtual void Destroy();

	virtual bool CreateShader() = 0;
	virtual bool CreateDescriptors() = 0;
	virtual bool CreateRootSignature() = 0;
	virtual bool CreatePSO() = 0;
	virtual bool CreateCommandBuffer() = 0;

	virtual void RefreshRenderCommandBuffer();

public:

	CommandBuffer* GetRenderCommandBuffer(uint32_t index) { return m_renderCommandBuffers.GetCommandBuffer(index); }

	SimpleShader* GetShader() { return m_shader; }
	ID3D12RootSignature* GetRootSignature() { return m_rootSignature; }
	ID3D12PipelineState* GetPSO() { return m_pso; }

public:
	void SetRenderObject(RenderObject* renderObj) { m_renderObj = renderObj; }

protected:

	StaticCommandBufferContainer m_renderCommandBuffers;

	StructuredBuffer<GlobalConstants>* m_globalConstntBuffer;

	ID3D12RootSignature* m_rootSignature;
	ID3D12PipelineState* m_pso;
	SimpleShader* m_shader;

	const CD3DX12_STATIC_SAMPLER_DESC m_linearWrapSampler =
	{
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	};

	RenderObject* m_renderObj;
};

class RenderItem_SkeletalMeshInstancing : public RenderItem
{
public:
	virtual ~RenderItem_SkeletalMeshInstancing() {}
public:
	virtual bool Initialize(RenderObject* renderObj,
							StructuredBuffer<GlobalConstants>* globalConstantBuffer,
							uint32_t frameCount = 2) override;
	virtual void Destroy() override;

	virtual void UpdateResources(CommandBuffer* cmdBuf) override;
	void UpdateInstanceResources(CommandBufferBase* cmdBuf);

	virtual bool CreateBuffers();
	virtual bool CreateDescriptors() override;

public:
	CommandBufferBase* GetAnimUpdateCommandBuffer(int frameIndex);

protected:

	int GetMaterialBindIndex(SimpleMaterial* material);
	int GetTextureBindIndex(SimpleTexture2D* texture);

protected:

	AnimationUpdator m_animUpdator;

	ID3D12DescriptorHeap* m_textureDescriptorHeap = nullptr;

	StructuredBuffer<InstanceConstants> m_instanceDatasBuffer;
	std::vector<InstanceConstants> m_instanceDatas;

	StructuredBuffer<MaterialConstants> m_materialDatasBuffer;
	std::vector<MaterialConstants> m_materialDatas;

	std::vector<SimpleMaterial*> m_materials;
	std::map<UID, SimpleTexture2D*> m_textures;
};

class RenderItem_HardwareInstancing : public RenderItem_SkeletalMeshInstancing
{
public:
	virtual ~RenderItem_HardwareInstancing() {}
public:
	virtual bool CreateShader() override;
	virtual bool CreateRootSignature() override;
	virtual bool CreatePSO() override;
	virtual bool CreateCommandBuffer()  override;
private:
	static const wchar_t* HARDWARE_INSTANCING_VS_FILE_PATH;
	static const wchar_t* HARDWARE_INSTANCING_PS_FILE_PATH;
};

class RenderItem_MeshShaderInstancing : public RenderItem_SkeletalMeshInstancing
{
public:
	virtual ~RenderItem_MeshShaderInstancing() {}

public:
	virtual bool CreateShader() override;
	virtual bool CreateRootSignature() override;
	virtual bool CreatePSO() override;
	virtual bool CreateCommandBuffer()  override;

private:
	static const wchar_t* MESHSHADER_INSTANCING_AS_FILE_PATH;
	static const wchar_t* MESHSHADER_INSTANCING_MS_FILE_PATH;
	static const wchar_t* MESHSHADER_INSTANCING_PS_FILE_PATH;
	
	static const uint32_t MAX_INSTANCE_COUNT_PER_AS = 64;
};