
#include "AnimationUpdator.h"
#include "SimpleShader.h"
#include "DX12DeviceResourceAccess.h"

bool AnimationUpdator::Initialize(SkeletalMeshRenderObject* renderObj, CommandBufferBase* cmdBuffer, uint32_t frameCount)
{
	if (renderObj == nullptr)
	{
		REPORT(EReportType::REPORT_TYPE_WARN, "Render Object is null.");
		return false;
	}
	m_renderObj = renderObj;
	m_numFrames = frameCount;
	
	m_animUpdateCommandBuffer.Initialize(frameCount);

	uint32_t numInstances = m_renderObj->GetInstanceCount();
	for (uint32_t i = 0; i < numInstances; i++)
	{
		SkeletalMeshRenderObjectInstance* curSkelMeshRenderObjInst = dynamic_cast<SkeletalMeshRenderObjectInstance*>(m_renderObj->GetInstance(i));
		if (curSkelMeshRenderObjInst != nullptr)
		{
			SkeletalAnimation* anim = curSkelMeshRenderObjInst->GetAnimation();
			SkeletalAnimationSequence* animSeq = anim->GetAnimationSequence();
			if (animSeq != nullptr)
			{
				UID animSeqUID = animSeq->GetUID();
				auto iterFind = m_animations.find(animSeqUID);
				if (iterFind == m_animations.end())
				{
					m_animations.insert(std::make_pair(animSeqUID, animSeq));
				}
			}
			else
			{
				REPORT(EReportType::REPORT_TYPE_POPUP_MESSAGE, "??");
			}
		}
	}

	//다수 스켈레탈 메시 케이스 나중에 추가하자
	//일단 틀을 잡아놓음
	SimpleSkeleton* skel = m_renderObj->GetSkeleton();
	m_skeletons.insert(std::make_pair(skel->GetUID(), skel));

	if (!CreateBuffers(cmdBuffer))
	{
		return false;
	}

	if (!CreateShader())
	{
		return false;
	}

	if (!CreateRootSignature())
	{
		return false;
	}

	if (!CreatePSO())
	{
		return false;
	}

	CreateCommandBuffer();

	return true;
}

void AnimationUpdator::UpdateResources(CommandBufferBase* cmdBuffer)
{
	//이것도 역시 render obj 여러 개 추가되면 for문 추가
	//다수의 렌더오브젝트에 대해서 skeleton bind index와 bone count로 옵셋 만들자
	uint32_t instCount = m_renderObj->GetInstanceCount();
	for (uint32_t i = 0; i < instCount; i++)
	{
		uint32_t skeletonOffset = 0;
		SkeletalMeshRenderObjectInstance* curInst = dynamic_cast<SkeletalMeshRenderObjectInstance*>(m_renderObj->GetInstance(i));
		SkeletalAnimationSequence* animSequnece = curInst->GetAnimation()->GetAnimationSequence();
		if (curInst != nullptr && animSequnece != nullptr)
		{
			m_animUpdateInstances[i].Duration = curInst->GetAnimation()->GetDuration();
			m_animUpdateInstances[i].Time = curInst->GetAnimation()->GetCurTime();
			m_animUpdateInstances[i].SkeletonOffset = skeletonOffset;
			m_animUpdateInstances[i].KeyCount = animSequnece->GetKeyCount();
		}
	}
	m_animUpdateInstanceBuffer.UploadResource(cmdBuffer, static_cast<void*>(m_animUpdateInstances.data()));
}

void AnimationUpdator::Refresh(CommandBufferBase* cmdBuffer)
{

}

void AnimationUpdator::Destroy()
{
	m_animUpdateCommandBuffer.Destory();

	m_animUpdateRootSignature->Release();
	m_animUpdatePso->Release();
	if (m_animUpdateShader != nullptr)
	{
		delete m_animUpdateShader;
	}

	m_skelUpdateRootSignature->Release();
	m_skelUpdatePso->Release();
	if (m_skelUpdateShader != nullptr)
	{
		delete m_skelUpdateShader;
	}

	m_animUpdateInstanceBuffer.Destroy();
	m_animUpdateInstancePerBoneBuffer.Destroy();
	m_skeletonBuffer.Destroy();
	m_animationBuffer.Destroy();
	m_animationUpdateBuffer.Destroy();
	m_hierarchyUpdateBuffer.Destroy();
	m_skeltonUpdateInstanceBuffer.Destroy();
}

bool AnimationUpdator::CreateBuffers(CommandBufferBase* cmdBuffer)
{
	if (m_renderObj == nullptr)
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Render object is null.");
		return false;
	}
	
	SimpleSkeleton* skeleton = m_renderObj->GetSkeleton();
	if (m_skeletons.size() == 0)
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Skeleton count is zero.");

		return false;
	}

	//여기서 전체 렌더오브젝트 인스턴스들의 카운트를 받도록 확장하자
	uint32_t renderObjectCount = 1;
	uint32_t instCount = renderObjectCount * m_renderObj->GetInstanceCount();
	m_countOfInstPerBone = instCount * skeleton->GetBoneCount();
	m_animUpdateInstances.resize(instCount);
	if (!m_animUpdateInstanceBuffer.Initialize(instCount, false, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ))
	{
		return false;
	}
	m_animUpdateInstanceBuffer.UploadResource(cmdBuffer, static_cast<void*>(m_animUpdateInstances.data()));

	std::vector<AnimUpdateInstancePerBone> animInstPerBoneList;
	animInstPerBoneList.resize(m_countOfInstPerBone);
	//renderObj당 for문
	//cur skeleton으로 변경하자.
	for (uint32_t i = 0; i < instCount; i++)
	{
		SkeletalMeshRenderObjectInstance* curInst = dynamic_cast<SkeletalMeshRenderObjectInstance*>(m_renderObj->GetInstance(i));
		if (curInst != nullptr)
		{
			SimpleSkeleton* curSkeleton = curInst->GetSkeleton();
			SkeletalAnimationSequence* curAnimSeq = curInst->GetAnimation()->GetAnimationSequence();
			int curAnimBindIndex = GetAnimBindIndex(curAnimSeq);
			//uint32_t curAnimFrameCount = curAnimSeq->GetNumFrames();
			uint32_t curAnimOffset = 0;

			for (int j = 0; j < curAnimBindIndex; j++)
			{
				auto pos = m_animations.begin();
				std::advance(pos, j);
				curAnimOffset += pos->second->GetAllAnimKeyCount();
			}

			if (curSkeleton != nullptr && curAnimBindIndex != - 1)
			{
				uint32_t firstLayerIndex = 0;
				SkeletalAnimationLayer* curLayer = curAnimSeq->GetAnimLayer(firstLayerIndex);

				uint32_t boneCount = curSkeleton->GetBoneCount();
				uint32_t animOffset = 0;
				for (uint32_t j = 0; j < boneCount; j++)
				{
					Bone* curBone = curSkeleton->GetBone(j);
					if (curBone != nullptr)
					{
						uint32_t instPerBoneIndex = i * boneCount + j;
						
						animInstPerBoneList[instPerBoneIndex].BoneIndex = j;
						animInstPerBoneList[instPerBoneIndex].AnimUpdateInstanceIndex = i;

						if (curLayer->GetKeyCount() > 0)
						{
							animInstPerBoneList[instPerBoneIndex].HasAnim = 1;
							animInstPerBoneList[instPerBoneIndex].AnimDataOffset = curAnimOffset + animOffset;
						}
						else
						{
							animInstPerBoneList[instPerBoneIndex].HasAnim = 0;
							animInstPerBoneList[instPerBoneIndex].AnimDataOffset = 0;
						}
						AnimationPerBone* animPerBone = curLayer->GetAnimationPerBone(curBone->GetName());
						if (animPerBone != nullptr)
						{
							animOffset += static_cast<uint32_t>(animPerBone->AnimKeys.size());
						}
					}
				}
			}
		}
	}

	if (!m_animUpdateInstancePerBoneBuffer.Initialize(m_countOfInstPerBone, false, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON))
	{
		return false;
	}
	m_animUpdateInstancePerBoneBuffer.UploadResource(cmdBuffer, static_cast<void*>(animInstPerBoneList.data()));
	std::vector<AnimKey> animKeys;
	for (auto& cur : m_animations)
	{
		if (cur.second != nullptr)
		{
			uint32_t layerCount = cur.second->GetAnimLayerCount();
			for (uint32_t i = 0; i < layerCount; i++)
			{
				SkeletalAnimationLayer* animLayer = cur.second->GetAnimLayer(i);
				if (animLayer != nullptr)
				{
					uint32_t animPerBoneCount = animLayer->GetAnimationPerBoneCount();
					for (uint32_t j = 0; j < animPerBoneCount; j++)
					{
						AnimationPerBone* curAnimPerBone = animLayer->GetAnimationPerBone(j);					
						animKeys.insert(animKeys.end(), curAnimPerBone->AnimKeys.begin(), curAnimPerBone->AnimKeys.end());
					}
				}
			}
		}
	}

	uint32_t animKeyCount = static_cast<uint32_t>(animKeys.size());
	if (!m_animationBuffer.Initialize(animKeyCount, false))
	{
		return false;
	}
	m_animationBuffer.UploadResource(cmdBuffer, reinterpret_cast<void*>(animKeys.data()));

	std::vector<TransformData> transformDatas;
	for (auto& cur : m_skeletons)
	{
		transformDatas.insert(transformDatas.end(), cur.second->GetTransformDatas().begin(), cur.second->GetTransformDatas().end());
	}

	if (!m_skeletonBuffer.Initialize(static_cast<uint32_t>(transformDatas.size()), false))
	{
		return false;
	}
	m_skeletonBuffer.UploadResource(cmdBuffer, reinterpret_cast<void*>(transformDatas.data()));

	uint32_t boneCount = m_renderObj->GetSkeleton()->GetBoneCount(); //다수의 렌더 오브젝트에 대한 걸로 변경할때 수정하자
	if (!m_animationUpdateBuffer.Initialize(instCount * boneCount, false, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON))
	{
		return false;
	}

	std::vector<XMFLOAT4X4> defaultMatrixs(instCount * boneCount, IDENTITY_FLOAT4X4);
	if (!m_hierarchyUpdateBuffer.Initialize(instCount * boneCount, false, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON))
	{
		return false;
	}
	m_hierarchyUpdateBuffer.UploadResource(cmdBuffer, reinterpret_cast<void*>(defaultMatrixs.data()));

	m_numSkeletonInstances = instCount * renderObjectCount;
	std::vector<SkeletonUpdateInstance> skeletonUpdateInstances;
	skeletonUpdateInstances.resize(m_numSkeletonInstances);

	//멀티 메쉬 인스턴싱 할 때 렌더 오브젝트 반복문 여기 추가
	//for renderobjlist
	//{
	SimpleSkeleton* curSkeleton = m_renderObj->GetSkeleton();
	uint32_t curRenderObjInstCount = m_renderObj->GetInstanceCount();
	uint32_t curSkelBoneCount = curSkeleton->GetBoneCount();
	uint32_t renderObjIdx = 0;
	uint32_t skelInstOffsetPerRenderObj = 0;
	for (uint32_t i = 0; i < m_numSkeletonInstances; i++)
	{
		//uint32_t curSkelInstIndex = renderObjIdx * curRenderObjInstCount + i; 이건 나중에 처리하자
		skeletonUpdateInstances[i].NumBones = curSkelBoneCount;
		skeletonUpdateInstances[i].SkeletonOffset = i * curSkelBoneCount;
	}
	//skelInstOffsetPerRenderObj += curRenderObjInstCount * curSkelBoneCount;
	//}

	if (!m_skeltonUpdateInstanceBuffer.Initialize(m_numSkeletonInstances, false))
	{
		return false;
	}
	m_skeltonUpdateInstanceBuffer.UploadResource(cmdBuffer, reinterpret_cast<void*>(skeletonUpdateInstances.data()));

	return true;
}

bool AnimationUpdator::CreateShader()
{	
	m_animUpdateShader = new SimpleShader(static_cast<SimpleShader::ShaderStageStates>(EShaderStageType::COMPUTE_SHADER));
	if (m_animUpdateShader != nullptr)
	{
		m_animUpdateShader->LoadCompiledShader(EShaderStageType::COMPUTE_SHADER, L"../Resources/Shaders/AnimationUpdate/AnimUpdate_CS.cso");
	}

	m_skelUpdateShader = new SimpleShader(static_cast<SimpleShader::ShaderStageStates>(EShaderStageType::COMPUTE_SHADER));
	if (m_skelUpdateShader != nullptr)
	{
		m_skelUpdateShader->LoadCompiledShader(EShaderStageType::COMPUTE_SHADER, L"../Resources/Shaders/AnimationUpdate/SkelUpdate_CS.cso");
	}

	return true;
}


bool AnimationUpdator::CreateRootSignature()
{
	HRESULT res = gLogicalDevice->CreateRootSignature
	(
		0,
		m_animUpdateShader->GetShader(EShaderStageType::COMPUTE_SHADER)->GetBufferPointer(),
		m_animUpdateShader->GetShader(EShaderStageType::COMPUTE_SHADER)->GetBufferSize(),
		IID_PPV_ARGS(&m_animUpdateRootSignature)
	);
	if (FAILED(res))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Animation update root signature create failed.");
		return false;
	}

	gLogicalDevice->CreateRootSignature
	(
		0,
		m_skelUpdateShader->GetShader(EShaderStageType::COMPUTE_SHADER)->GetBufferPointer(),
		m_skelUpdateShader->GetShader(EShaderStageType::COMPUTE_SHADER)->GetBufferSize(),
		IID_PPV_ARGS(&m_skelUpdateRootSignature)
	);
	if (FAILED(res))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Skeleton update root signature create failed.");
		return false;
	}

	return true;
}

bool AnimationUpdator::CreatePSO()
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC animUpdatePSD = {};
	animUpdatePSD.pRootSignature = m_animUpdateRootSignature;
	animUpdatePSD.CS =
	{
		reinterpret_cast<BYTE*>(m_animUpdateShader->GetShader(EShaderStageType::COMPUTE_SHADER)->GetBufferPointer()),
		m_animUpdateShader->GetShader(EShaderStageType::COMPUTE_SHADER)->GetBufferSize()
	};
	animUpdatePSD.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	HRESULT res = gLogicalDevice->CreateComputePipelineState(&animUpdatePSD, IID_PPV_ARGS(&m_animUpdatePso));
	if (FAILED(res))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Animation update compute pso create failed.");
		return false;
	}

	D3D12_COMPUTE_PIPELINE_STATE_DESC skelUpdatePSD = {};
	skelUpdatePSD.pRootSignature = m_skelUpdateRootSignature;
	skelUpdatePSD.CS =
	{
		reinterpret_cast<BYTE*>(m_skelUpdateShader->GetShader(EShaderStageType::COMPUTE_SHADER)->GetBufferPointer()),
		m_skelUpdateShader->GetShader(EShaderStageType::COMPUTE_SHADER)->GetBufferSize()
	};
	skelUpdatePSD.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	res = gLogicalDevice->CreateComputePipelineState(&skelUpdatePSD, IID_PPV_ARGS(&m_skelUpdatePso));
	if (FAILED(res))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Skeleton update compute pso create failed.");
		return false;
	}

	return true;
}

bool AnimationUpdator::CreateCommandBuffer()
{
	for (uint32_t i = 0; i < m_numFrames; i++)
	{
		CommandBuffer* animUpdateCmdBuf = m_animUpdateCommandBuffer.GetCommandBuffer(i);
		animUpdateCmdBuf->GetCommandBuffer()->SetName(L"anim cmd buf");
		animUpdateCmdBuf->GetCommandBuffer()->SetName(L"skel cmd buf");

		if (animUpdateCmdBuf->Begin(m_animUpdatePso))
		{
			animUpdateCmdBuf->GetCommandBuffer()->ResourceBarrier
			(
				1,
				&CD3DX12_RESOURCE_BARRIER::Transition
				(
					m_animationUpdateBuffer.GetResource(),
					D3D12_RESOURCE_STATE_COMMON,
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS
				)
			);

			animUpdateCmdBuf->GetCommandBuffer()->ResourceBarrier
			(
				1,
				&CD3DX12_RESOURCE_BARRIER::Transition
				(
					m_hierarchyUpdateBuffer.GetResource(),
					D3D12_RESOURCE_STATE_COMMON,
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS
				)
			);
			
			animUpdateCmdBuf->GetCommandBuffer()->SetComputeRootSignature(m_animUpdateRootSignature);
			animUpdateCmdBuf->GetCommandBuffer()->SetComputeRootShaderResourceView(0, m_animUpdateInstanceBuffer.GetResource()->GetGPUVirtualAddress());
			animUpdateCmdBuf->GetCommandBuffer()->SetComputeRootShaderResourceView(1, m_animUpdateInstancePerBoneBuffer.GetResource()->GetGPUVirtualAddress());
			animUpdateCmdBuf->GetCommandBuffer()->SetComputeRootShaderResourceView(2, m_animationBuffer.GetResource()->GetGPUVirtualAddress());
			animUpdateCmdBuf->GetCommandBuffer()->SetComputeRootShaderResourceView(3, m_skeletonBuffer.GetResource()->GetGPUVirtualAddress());
			animUpdateCmdBuf->GetCommandBuffer()->SetComputeRootUnorderedAccessView(4, m_animationUpdateBuffer.GetResource()->GetGPUVirtualAddress());

			uint32_t dispatchCount = m_countOfInstPerBone / MAX_DISPATCH_GROUP_COUNT + 1;
			uint32_t dispatchOffset = 0;
			for (uint32_t i = 0; i < dispatchCount; i++)
			{
				uint32_t dispatchThreadCount = min(m_countOfInstPerBone - dispatchOffset, MAX_DISPATCH_GROUP_COUNT);
				animUpdateCmdBuf->GetCommandBuffer()->SetComputeRoot32BitConstant(5, dispatchOffset, 0);
				animUpdateCmdBuf->GetCommandBuffer()->SetComputeRoot32BitConstant(5, m_countOfInstPerBone, 1);
				animUpdateCmdBuf->GetCommandBuffer()->Dispatch(DivRoundUp(dispatchThreadCount, THREAD_GROUP_SIZE), 1, 1);
				dispatchOffset += dispatchThreadCount;
			}

			animUpdateCmdBuf->GetCommandBuffer()->SetPipelineState(m_skelUpdatePso);
			animUpdateCmdBuf->GetCommandBuffer()->SetComputeRootSignature(m_skelUpdateRootSignature);
			animUpdateCmdBuf->GetCommandBuffer()->SetComputeRootShaderResourceView(0, m_skeltonUpdateInstanceBuffer.GetResource()->GetGPUVirtualAddress());
			animUpdateCmdBuf->GetCommandBuffer()->SetComputeRootUnorderedAccessView(1, m_animationUpdateBuffer.GetResource()->GetGPUVirtualAddress());
			animUpdateCmdBuf->GetCommandBuffer()->SetComputeRootUnorderedAccessView(2, m_hierarchyUpdateBuffer.GetResource()->GetGPUVirtualAddress());

			dispatchCount = m_numSkeletonInstances / MAX_DISPATCH_GROUP_COUNT + 1;
			dispatchOffset = 0;
			for (uint32_t i = 0; i < dispatchCount; i++)
			{
				uint32_t dispatchThreadCount = min(m_numSkeletonInstances - dispatchOffset, MAX_DISPATCH_GROUP_COUNT);
				animUpdateCmdBuf->GetCommandBuffer()->SetComputeRoot32BitConstant(3, dispatchOffset, 0);
				animUpdateCmdBuf->GetCommandBuffer()->SetComputeRoot32BitConstant(3, m_countOfInstPerBone, 1);
				animUpdateCmdBuf->GetCommandBuffer()->Dispatch(DivRoundUp(m_numSkeletonInstances, THREAD_GROUP_SIZE), 1, 1);
				dispatchOffset += dispatchThreadCount;
			}
			
			animUpdateCmdBuf->GetCommandBuffer()->ResourceBarrier
			(
				1,
				&CD3DX12_RESOURCE_BARRIER::Transition
				(
					m_animationUpdateBuffer.GetResource(),
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
					D3D12_RESOURCE_STATE_COMMON
				)
			);

			animUpdateCmdBuf->GetCommandBuffer()->ResourceBarrier
			(
				1,
				&CD3DX12_RESOURCE_BARRIER::Transition
				(
					m_hierarchyUpdateBuffer.GetResource(),
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
					D3D12_RESOURCE_STATE_COMMON
				)
			);
			
			animUpdateCmdBuf->End();
		}
	}
	return true;
}

void AnimationUpdator::RefreshRenderCommandBuffer()
{
	
}

int AnimationUpdator::GetAnimBindIndex(SkeletalAnimationSequence* animSeq)
{
	if (animSeq != nullptr)
	{
		auto iterFind = m_animations.find(animSeq->GetUID());
		if (iterFind != m_animations.end())
		{
			return static_cast<int>(std::distance(m_animations.begin(), iterFind));
		}
	}
	return -1;
}