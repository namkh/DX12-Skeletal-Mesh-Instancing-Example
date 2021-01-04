#pragma once

#include "RenderObject.h"
#include "DeviceBuffers.h"

struct AnimUpdateInstance
{
	float Time = 0;
	float Duration = 0;
	uint32_t SkeletonOffset = 0;
	uint32_t KeyCount = 0;
};

struct AnimUpdateInstancePerBone
{
	uint32_t AnimDataOffset = 0;
	uint32_t BoneIndex = 0;
	uint32_t AnimUpdateInstanceIndex = 0;
	uint32_t HasAnim = 0;
};

struct SkeletonUpdateInstance
{
	uint32_t SkeletonOffset = 0;
	uint32_t NumBones = 0;
};

class AnimationUpdator
{
public:
	~AnimationUpdator() {}
public:
	bool Initialize(SkeletalMeshRenderObject* renderObj, CommandBufferBase* cmdBuffer, uint32_t frameCount = 2);
	void UpdateResources(CommandBufferBase* cmdBuffer);
	void Refresh(CommandBufferBase* cmdBuffer);
	void Destroy();

	bool CreateBuffers(CommandBufferBase* cmdBuffer);
	bool CreateShader();
	bool CreateRootSignature();
	bool CreatePSO();
	bool CreateCommandBuffer();

	void RefreshRenderCommandBuffer();

public:
	CommandBuffer* GetAnimUpdateCommandBuffer(uint32_t index) { return m_animUpdateCommandBuffer.GetCommandBuffer(index); }

	void SetRenderObject(SkeletalMeshRenderObject* renderObj) { m_renderObj = renderObj; }
	int GetAnimBindIndex(SkeletalAnimationSequence* animSeq);

	StructuredBuffer<XMFLOAT4X4>& GetResultSkeletonBuffer() { return m_hierarchyUpdateBuffer; }

protected:
	StaticCommandBufferContainer m_animUpdateCommandBuffer{};

	ID3D12RootSignature* m_animUpdateRootSignature = nullptr;
	ID3D12PipelineState* m_animUpdatePso = nullptr;
	SimpleShader* m_animUpdateShader = nullptr;

	ID3D12RootSignature* m_skelUpdateRootSignature = nullptr;
	ID3D12PipelineState* m_skelUpdatePso = nullptr;
	SimpleShader* m_skelUpdateShader = nullptr;

	SkeletalMeshRenderObject* m_renderObj = nullptr;

	std::vector<AnimUpdateInstance> m_animUpdateInstances;
	StructuredBuffer<AnimUpdateInstance> m_animUpdateInstanceBuffer = {};
	StructuredBuffer<AnimUpdateInstancePerBone> m_animUpdateInstancePerBoneBuffer = {};
	StructuredBuffer<TransformData> m_skeletonBuffer = {};
	StructuredBuffer<AnimKey>  m_animationBuffer = {};

	StructuredBuffer<TransformData> m_animationUpdateBuffer = {};
	StructuredBuffer<XMFLOAT4X4> m_hierarchyUpdateBuffer = {};

	StructuredBuffer<SkeletonUpdateInstance> m_skeltonUpdateInstanceBuffer = {};

	std::unordered_map<UID, SkeletalAnimationSequence*> m_animations;
	std::unordered_map<UID, SimpleSkeleton*> m_skeletons;

	uint32_t m_numSkeletonInstances = 0;
	uint32_t m_countOfInstPerBone = 0;
	uint32_t m_numFrames = 0;

	static const uint32_t THREAD_GROUP_SIZE = 64;
};