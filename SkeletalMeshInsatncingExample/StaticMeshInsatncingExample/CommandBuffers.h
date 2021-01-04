#pragma once

#include <vector>
#include <queue>

#include "Defaults.h"
#include "Commands.h"
#include "Fence.h"
#include "Singleton.h"
#include "Utils.h"

class CommandAllocator : public UniqueIdentifier
{
public:
	CommandAllocator();
	~CommandAllocator();
	ID3D12CommandAllocator* GetAllocator();
	void Reset();

private:
	ID3D12CommandAllocator* m_allocator;
};

class CommandAllocatorManager : public TSingleton<CommandAllocatorManager>
{
public:
	CommandAllocatorManager(token)
	{
		
	};

public:
	CommandAllocator* GetCommandAllocator();
	void ReleaseCommandAllocator(CommandAllocator* allocator);
	void Destory();

public:
	std::map<UID, uint32_t> m_bindTable;
	std::vector<CommandAllocator*> m_commandAllocatorList;
	std::queue<uint32_t> m_idleAllocatorIndexQueue;
};

#define gCmdAllocMgr CommandAllocatorManager::Instance()

class CommandBufferBase
{
public:
	virtual ~CommandBufferBase() {}
public:
	void FreeCommandBuffers();
	void Reset();
protected:
	bool AllocateCommandBuffer();
	virtual bool Begin();
	virtual bool Begin(ID3D12PipelineState* pso);
	virtual bool End();

public:
	ID3D12GraphicsCommandList* GetCommandBuffer()
	{
		if (m_isInitialized)
		{
			return m_commandBuffer;
		}
		return nullptr;
	};

protected:

	ID3D12GraphicsCommandList* m_commandBuffer;
	CommandAllocator* m_commandAllocator;
	ID3D12CommandQueue* m_commandQueue;
	bool m_isInitialized = false;
};

class CommandBuffer : public CommandBufferBase
{
public:
	virtual ~CommandBuffer() {}
public:
	enum class ECommandBufferState
	{
		COMMAND_BUFFER_STATE_READY,
		COMMAND_BUFFER_STATE_RECORDED,
		COMMAND_BUFFER_STATE_SUBMITTED,
	};
public:
	bool Initialize();
	virtual bool Begin(ID3D12PipelineState* pso) override;
	virtual bool End() override;

	ECommandBufferState GetState() { return m_state; }
	void OnSubmitted() { m_state = ECommandBufferState::COMMAND_BUFFER_STATE_SUBMITTED; }
	void OnExecutionEnded() { m_state = ECommandBufferState::COMMAND_BUFFER_STATE_READY; }

private:

	ECommandBufferState m_state = ECommandBufferState::COMMAND_BUFFER_STATE_READY;
};

class StaticCommandBufferContainer
{
public:
	bool Initialize(uint32_t allocCount);
	void Destory();
	CommandBuffer* GetCommandBuffer(uint32_t index);
	void Clear();

	uint32_t GetCommandBufferCount() { return static_cast<uint32_t>(m_commandList.size()); }

private:
	std::vector<CommandBuffer> m_commandList;
};

class DynamicCommandBufferContainer
{
public:
	bool Initialize();
	void Destory();
	CommandBuffer* GetCommandBuffer();
	CommandBuffer* GetCommandBuffer(uint32_t index);
	void Clear();

	uint32_t GetCommandBufferCount() { return static_cast<uint32_t>(m_commandList.size()); }

private:

	std::vector<CommandBuffer*> m_commandList;
};

class SingleTimeCommandBuffer : public CommandBufferBase
{
public:
	virtual ~SingleTimeCommandBuffer() {}
public:
	virtual bool Begin() override;
	virtual bool Begin(ID3D12PipelineState* pso) override;
	virtual bool End() override;

private:
	Fence m_fence = {};
};