#include "SkeletalMeshInstancingExample.h"
#include "DX12DeviceResourceAccess.h"
#include "RenderObjectContainer.h"
#include "GeometryContainer.h"
#include "TextureContainer.h"
#include "MaterialContainer.h"
#include "SkeletonContainer.h"
#include "AnimSequenceContainer.h"
#include "GlobalSystemValues.h"
#include "Utils.h"

#include <random>

bool SkeletalMeshInstancingExample::Initialize()
{
	m_camera.Initialize
	(
		static_cast<float>(m_width), 
		static_cast<float>(m_height),
		GlobalSystemValues::Instance().ViewportNearDistance, 
		GlobalSystemValues::Instance().ViewportFarDistance,
		GlobalSystemValues::Instance().FovAngleY
	);
	m_camera.SetRadius(400.0f);
	m_camera.SetSeta(PI * -0.5f);
	m_camera.SetPhi(PI * 0.5f);
	m_camera.SetZoomSensitivity(50.0f);

	gFbxLoader.Initialize();

	m_globalConstantsBuffer.Initialize(1, true);
	
	m_fences.resize(FRAME_COUNT);
	for (auto& cur : m_fences)
	{
		cur.Initialize(L"MainFence");
	}

	m_updateCommands.Initialize(FRAME_COUNT);
	
	if (!CreateRenderObjects())
	{
		return false;
	}
	m_screenSizeChangedCallbackHandle = gDX12DeviceRes->OnRenderTargetSizeChanged.Add
	(
		[this]()
		{
			OnScreenSizeChanged(gDX12DeviceRes->GetWidth(), gDX12DeviceRes->GetHeight());
		}
	);
	
	m_keyEventHandle = CoreEventManager::Instance().RegisterKeyboardEventCallback(this, &SkeletalMeshInstancingExample::OnKeyboardEvent);

	return true;
}

void SkeletalMeshInstancingExample::Update(float timeDelta)
{
	for (auto& cur : m_renderObjects)
	{
		if (cur != nullptr)
		{
			uint32_t instCount = cur->GetInstanceCount();
			for (uint32_t i = 0; i < instCount; i++)
			{
				RenderObjectInstance* curInstance = cur->GetInstance(i);
				if (curInstance != nullptr)
				{
					curInstance->Update(timeDelta);
				}
			}
		}
	}
}

void SkeletalMeshInstancingExample::PreRender()
{
	m_fences[m_currentFrame].WaitForEnd();

	m_globalConstants.MatViewProj = XMMatrixTranspose(m_camera.GetViewProjectionMatrix());
	m_globalConstants.CamPos = m_camera.GetPosition();

	CommandBuffer* cmdBuf = m_updateCommands.GetCommandBuffer(m_currentFrame);
	cmdBuf->Begin(nullptr);
	auto iterRenderItemFinded = m_renderItems.find(m_currentRenderItem);
	if (iterRenderItemFinded != m_renderItems.end())
	{
		iterRenderItemFinded->second->UpdateResources(cmdBuf);
	}
	
	m_globalConstantsBuffer.UploadResource(cmdBuf, reinterpret_cast<void*>(&m_globalConstants));
	cmdBuf->End();
}

void SkeletalMeshInstancingExample::Render()
{
	std::vector<CommandBufferBase*> submitCommands;
	for (auto& cur : m_updateCommandLists)
	{
		submitCommands.push_back(cur);
	}

	auto iterRenderItemFinded = m_renderItems.find(m_currentRenderItem);
	if (iterRenderItemFinded != m_renderItems.end())
	{
		if (iterRenderItemFinded->second != nullptr)
		{
			std::vector<CommandBuffer*> updateCommandBuffers;
			CommandBuffer* cmdBuf = m_updateCommands.GetCommandBuffer(m_currentFrame);
			if (cmdBuf != nullptr && cmdBuf->GetState() == CommandBuffer::ECommandBufferState::COMMAND_BUFFER_STATE_RECORDED)
			{
				submitCommands.push_back(cmdBuf);
			}
			
			RenderItem_SkeletalMeshInstancing* skeletalMeshInstancing = dynamic_cast<RenderItem_SkeletalMeshInstancing*>(iterRenderItemFinded->second);
			if (skeletalMeshInstancing != nullptr)
			{
				CommandBufferBase* animUpdateCmdBuf = skeletalMeshInstancing->GetAnimUpdateCommandBuffer(m_currentFrame);
				if (animUpdateCmdBuf != nullptr)
				{
					submitCommands.push_back(animUpdateCmdBuf);
				}
			}

			CommandBuffer* renderItemCmdBuf = iterRenderItemFinded->second->GetRenderCommandBuffer(m_currentFrame);
			if (renderItemCmdBuf != nullptr)
			{
			 	submitCommands.push_back(renderItemCmdBuf);
			}
		}
	}

	if (submitCommands.size() > 0)
	{
		gDX12DeviceRes->GetDefaultCommandQueue().Submit(submitCommands, &m_fences[m_currentFrame], true);
	}

	m_currentFrame = (m_currentFrame + 1) % FRAME_COUNT;
}

void SkeletalMeshInstancingExample::Destroy()
{
	CoreEventManager::Instance().UnregisterEventCallback(m_keyEventHandle);
	gDX12DeviceRes->OnRenderTargetSizeChanged.Remove(m_screenSizeChangedCallbackHandle);
	gDX12DeviceRes->GetDefaultCommandQueue().WaitForAllFence();
	
	m_camera.Destroy();
	for (auto& cur : m_renderItems)
	{
		if (cur.second != nullptr)
		{
			cur.second->Destroy();
			delete cur.second;
		}
	}
	m_renderItems.clear();
	for (auto& cur : m_fences)
	{
		cur.Destory();
	}
	for (auto& cur : m_animSequences)
	{
		cur->Destroy();
	}

	m_updateCommands.Destory();

	m_globalConstantsBuffer.Destroy();

	gRenderObjContainer.Clear();
	gSkelContainer.Clear();
	gAnimSeqContainer.Clear();
	gGeomContainer.Clear();
	gMaterialContainer.Clear();
	gTexContainer.Clear();
	gCmdAllocMgr.Destory();
}

void SkeletalMeshInstancingExample::OnKeyboardEvent(KeyboardEvent* keyEvent)
{
	if (keyEvent->m_keyType == EKeyboaradEvent::SPACE)
	{
		if (m_currentRenderItem == ERenderItemType::MESH_SHADER_INSTANCING)
		{
			m_appName = L"Hardware Instancing";
			m_currentRenderItem = ERenderItemType::HARDWARE_INSTANCING;
		}
		else
		{
			m_appName = L"Mesh Shader Instancing";
			m_currentRenderItem = ERenderItemType::MESH_SHADER_INSTANCING;
		}
	}
}

void SkeletalMeshInstancingExample::OnScreenSizeChanged(uint32_t width, uint32_t height)
{
	for (auto& cur : m_renderItems)
	{
		if (cur.second != nullptr)
		{
			cur.second->RefreshRenderCommandBuffer();
		}
	}
	m_currentFrame = 0;
}

bool SkeletalMeshInstancingExample::CreateRenderObjects()
{
	SingleTimeCommandBuffer singleTimeCommandBuffer;
	singleTimeCommandBuffer.Begin();
	RenderObject* renderObj = gRenderObjContainer.CreateSkeletalMeshRenderObject(&singleTimeCommandBuffer, "../Resources/Mesh/Model.fbx", ExampleMaterialType::EXAMPLE_MAT_INSTANCING1, true);
	m_renderObjects.push_back(renderObj);
	SkeletalMeshRenderObject* skelMeshRenderObj = dynamic_cast<SkeletalMeshRenderObject*>(renderObj);
	if (skelMeshRenderObj != nullptr)
	{
		SimpleSkeleton* skel = skelMeshRenderObj->GetSkeleton();
		SkeletalAnimationSequence* anim = gAnimSeqContainer.CreateAnimSequence("Anim1", "../Resources/Animation/Anim1.fbx", skel);
		m_animSequences.push_back(anim);
		anim = gAnimSeqContainer.CreateAnimSequence("Anim2", "../Resources/Animation/Anim2.fbx", skel);
		m_animSequences.push_back(anim);
		anim = gAnimSeqContainer.CreateAnimSequence("Anim3", "../Resources/Animation/Anim3.fbx", skel);
		m_animSequences.push_back(anim);
		anim = gAnimSeqContainer.CreateAnimSequence("Anim4", "../Resources/Animation/Anim4.fbx", skel);
		m_animSequences.push_back(anim);
		anim = gAnimSeqContainer.CreateAnimSequence("Anim5", "../Resources/Animation/Anim5.fbx", skel);
		m_animSequences.push_back(anim);
		anim = gAnimSeqContainer.CreateAnimSequence("Anim6", "../Resources/Animation/Anim6.fbx", skel);
		m_animSequences.push_back(anim);
		anim = gAnimSeqContainer.CreateAnimSequence("Anim7", "../Resources/Animation/Anim7.fbx", skel);
		m_animSequences.push_back(anim);
		anim = gAnimSeqContainer.CreateAnimSequence("Anim8", "../Resources/Animation/Anim8.fbx", skel);
		m_animSequences.push_back(anim);
		anim = gAnimSeqContainer.CreateAnimSequence("Anim9", "../Resources/Animation/Anim9.fbx", skel);
		m_animSequences.push_back(anim);
		anim = gAnimSeqContainer.CreateAnimSequence("Anim10", "../Resources/Animation/Anim10.fbx", skel);
		m_animSequences.push_back(anim);
	}
	
	float startPosX = m_objectArea.x * -0.5f;
	float startPosY = m_objectArea.y * -0.5f;
	float startPosZ = m_objectArea.z * -0.5f;
	
	float intervalX = m_objectArea.x * (1.0f / 29.0f);
	float intervalY = m_objectArea.y * (1.0f / 19.0f);
	float intervalZ = m_objectArea.z * (1.0f / 19.0f);

	std::random_device rnd;
	std::mt19937_64 gen(rnd());

	int matIndex = 0;
	int animIndex = 0;
	for (uint32_t i = 0; i < m_objectCountZ; i++)
	{
		for (uint32_t j = 0; j < m_objectCountY; j++)
		{
			for (uint32_t k = 0; k < m_objectCountX; k++)
			{
				XMMATRIX matScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
				XMMATRIX matRotation = XMMatrixRotationY(PI);
				XMMATRIX matWorld = matScale * matRotation * XMMatrixTranslation(startPosX + k * intervalX, startPosY + j * intervalY, startPosZ + i * intervalZ);
				RenderObjectInstance* objInst = renderObj->CreateInstance(matWorld);
				
				switch (matIndex)
				{
					case 0:
						objInst->SetOverrideMaterial(&singleTimeCommandBuffer, ExampleMaterialType::EXAMPLE_MAT_INSTANCING1);
						break;
					case 1:
						objInst->SetOverrideMaterial(&singleTimeCommandBuffer, ExampleMaterialType::EXAMPLE_MAT_INSTANCING2);
						break;
					case 2:
						objInst->SetOverrideMaterial(&singleTimeCommandBuffer, ExampleMaterialType::EXAMPLE_MAT_INSTANCING3);
						break;
				}

				SkeletalMeshRenderObjectInstance* skelMeshObjInst = dynamic_cast<SkeletalMeshRenderObjectInstance*>(objInst);
				if (skelMeshObjInst != nullptr)
				{
					float duration = m_animSequences[animIndex]->GetDuration();
					std::uniform_real_distribution<float> dis(0.0f, duration);
					skelMeshObjInst->GetAnimation()->Play(true, dis(gen), m_animSequences[animIndex]);
					m_renderObjectInsts.push_back(skelMeshObjInst);
				}
			
				++matIndex;
				if (matIndex >= 3)
				{
					matIndex = 0;
				}

				++animIndex;
				if (animIndex >= 10)
				{
					animIndex = 0;
				}
			}
		}
	}
	
	RenderItem_HardwareInstancing* hardwareInstancingRenderItem = new RenderItem_HardwareInstancing();
	hardwareInstancingRenderItem->Initialize
	(
		renderObj, 
		&m_globalConstantsBuffer, 
		FRAME_COUNT
	);
	m_renderItems.insert(std::make_pair(ERenderItemType::HARDWARE_INSTANCING, hardwareInstancingRenderItem));

	RenderItem_MeshShaderInstancing* meshShaderInstancingRenderItem = new RenderItem_MeshShaderInstancing();
	meshShaderInstancingRenderItem->Initialize
	(
		renderObj,
		&m_globalConstantsBuffer,
		FRAME_COUNT
	);
	m_renderItems.insert(std::make_pair(ERenderItemType::MESH_SHADER_INSTANCING, meshShaderInstancingRenderItem));

	singleTimeCommandBuffer.End();
	return true;
}