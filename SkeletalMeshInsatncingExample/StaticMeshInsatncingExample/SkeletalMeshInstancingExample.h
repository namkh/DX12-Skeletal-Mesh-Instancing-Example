#pragma once

#include "ExampleAppBase.h"
#include "CommandBuffers.h"
#include "SimpleShader.h"
#include "RenderItem.h"
#include "SphericalCoordMovementCamera.h"
#include "SkeletalAnimation.h"

enum class ERenderItemType
{
	HARDWARE_INSTANCING,
	MESH_SHADER_INSTANCING
};

class SkeletalMeshInstancingExample : public ExampleAppBase
{
public:
	SkeletalMeshInstancingExample(std::wstring appName, int width, int height)
		: ExampleAppBase(appName, width, height)
	{
	}

	virtual ~SkeletalMeshInstancingExample() {}

public:
	virtual bool Initialize() override;
	virtual void Update(float timeDelta) override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void Destroy() override;

	void OnKeyboardEvent(KeyboardEvent* keyEvent);
	void OnScreenSizeChanged(uint32_t width, uint32_t height);

public:
	bool CreateRenderObjects();

public:
	StaticCommandBufferContainer m_updateCommands;

	std::vector<CommandBuffer*> m_updateCommandLists;
	std::vector<Fence> m_fences = {};

	GlobalConstants m_globalConstants;
	StructuredBuffer<GlobalConstants> m_globalConstantsBuffer;

	ERenderItemType m_currentRenderItem = ERenderItemType::MESH_SHADER_INSTANCING;
	std::map<ERenderItemType, RenderItem*> m_renderItems = {};
	std::vector<RenderObject*> m_renderObjects = {};
	std::vector<SkeletalMeshRenderObjectInstance*> m_renderObjectInsts = {};
	std::vector<SkeletalAnimationSequence*> m_animSequences = {};
	std::map<UID, SkeletalAnimationSequence*> m_animSequencesTest = {};

	SphericalCoordMovementCamera m_camera;
	XMFLOAT3 m_objectArea = XMFLOAT3(500.0f, 400.0f, 600.0f);
	uint32_t m_objectCountX = 25;
	uint32_t m_objectCountY = 20;
	uint32_t m_objectCountZ = 20;
	
	uint32_t m_currentFrame = 0;
	static const uint8_t FRAME_COUNT = 2;

	CoreEventHandle m_keyEventHandle = {};
	CommandHandle m_screenSizeChangedCallbackHandle = {};
};